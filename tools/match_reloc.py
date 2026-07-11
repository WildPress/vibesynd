#!/usr/bin/env python3
"""Relocation-aware match using the .obj's REAL OMF fixup records (tools/omf.py).

Masks exactly the relocated byte ranges in BOTH the target and our compiled output,
so relocations don't block a match: call/jmp rel32, abs32 data operands, AND
relocations that carry a NON-ZERO addend (array bounds, arr[const], loop `cmp`,
`((T*)addr)->field`) which the old all-zero-run masker missed.

  python3 tools/match_reloc.py <function-name>    (honors $WATFLAGS, $SKIP_COMPILE)
"""
import json, sys, subprocess, os
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MANIFEST = "manifest/functions.json"

def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)

def main():
    name = sys.argv[1]
    man = json.load(open(MANIFEST)); base = int(man.get("image_base", "0"), 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    if not os.environ.get("SKIP_COMPILE"):
        if subprocess.run(["bash", "tools/wcompile.sh", name]).returncode:
            sys.exit("compile failed")
    off = int(f["addr"], 16) - base
    tb = open(SEG, "rb").read()[off:off + f["size"]]
    ob, fixups = text_bytes_and_fixups(f"build/{name}.obj")
    tm, om = mask(tb, fixups), mask(ob, fixups)
    print(f"\n=== {name}  addr={f['addr']}  target={len(tb)}B  ours={len(ob)}B ===")
    print(f"target: {tb.hex()}")
    print(f"ours  : {ob.hex()}")
    print(f"fixups masked: {[(hex(o), s) for o, s in fixups]}")
    print(f"EXACT byte match          : {'YES' if tb == ob else 'NO'}")
    ok = (len(tb) == len(ob)) and (tm == om)
    print(f"RELOC-AWARE match (masked): {'YES ✅' if ok else 'NO'}")
    if not ok:
        n = min(len(tm), len(om))
        d = next((i for i in range(n) if tm[i] != om[i]), n)
        print(f"  first diff at 0x{d:x}: target={tm[d:d+1].hex()} ours={om[d:d+1].hex()}")

if __name__ == "__main__":
    main()
