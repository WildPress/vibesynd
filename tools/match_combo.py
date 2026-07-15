#!/usr/bin/env python3
"""match_combo.py -- verify a COMBINED translation unit (multiple functions in one .c,
compiled to one .obj) against the contiguous target region they occupy.

Some sibling functions were compiled together in the original and share code across the
function boundary (cross-function tail-merge: one function's return stub is reached by a
BACKWARD jump from the next). That backward jump only exists when both are in the same
object, so they live in one source file. This verifier compiles that file once and does a
whole-region, relocation-aware byte comparison covering ALL its functions + the inter-fn
padding -- so the cross-function branch is checked byte-exact (it is NOT a relocation).

A unit is identified by its PRIMARY (lowest-address) function name == the .c basename.
Member functions are the primary plus every manifest fn with "unit": <primary>.

  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/match_combo.py <primary>
"""
import json, sys, os, subprocess
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
FLAGS = os.environ.get("MATCH_FLAGS", "-4s -oneatx -zp8 -s -zq")


def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)


def main():
    primary = sys.argv[1]
    man = json.load(open("manifest/functions.json"))
    base = int(man.get("image_base", "0"), 16)
    fns = man["functions"]
    members = [f for f in fns if f["name"] == primary or f.get("unit") == primary]
    if not members:
        sys.exit("no unit members for " + primary)
    members.sort(key=lambda f: int(f["addr"], 16))
    first = int(members[0]["addr"], 16)
    last = members[-1]
    region_len = (int(last["addr"], 16) + last["size"]) - first
    print(f"unit {primary}: {[m['name'] for m in members]}  region={region_len}B @ {hex(first)}")

    if not os.environ.get("SKIP_COMPILE"):
        if subprocess.run(["bash", "tools/wcc_95.sh", primary, FLAGS]).returncode:
            sys.exit("compile failed")
    ob, fixups = text_bytes_and_fixups(f"build/{primary}.obj")
    tb = open(SEG, "rb").read()[first - base: first - base + region_len]

    n = min(len(ob), region_len)
    fx = [(o, s) for o, s in fixups if o < n]
    tm, om = mask(tb[:n], fx), mask(ob[:n], fx)
    ok = (len(ob) >= region_len) and (len(tb) == region_len) and tm == om
    print(f"target region {len(tb)}B  ours _TEXT {len(ob)}B")
    print(f"UNIT RELOC-AWARE match (masked): {'YES ✅' if ok else 'NO'}")
    if not ok:
        d = next((i for i in range(n) if tm[i] != om[i]), n)
        print(f"  first diff @ region off 0x{d:x} (addr {hex(first + d)}): "
              f"target={tm[d:d+1].hex()} ours={om[d:d+1].hex()}")
        print(f"  ours : {ob[max(0,d-3):d+6].hex()}")
        print(f"  tgt  : {tb[max(0,d-3):d+6].hex()}")
        sys.exit(1)
    # per-member confirmation (offsets by address arithmetic; region already byte-equal)
    for m in members:
        print(f"  MEMBER {m['name']} @ {m['addr']} ({m['size']}B): matched")

if __name__ == "__main__":
    main()
