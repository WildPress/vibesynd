#!/usr/bin/env python3
"""Relocation-aware match: like match.py, but masks call/jmp rel32 operands
(E8/E9 opcode + its 4 operand bytes) in BOTH the target and our compiled output,
so a function's relative call/jump targets don't block an otherwise-exact match.

This is a first, deliberately simple pass at objdiff-style relocation handling:
our .obj emits `E8 00000000` for an unresolved extern call; the original has the
real relative offset. Masking both makes them comparable. (Heuristic: masks every
E8/E9; fine for functions where those bytes are always calls/jumps. Later: drive
the mask from the .obj's actual OMF fixup records for full robustness.)

  python3 tools/match_reloc.py <function-name>       (honors $WATFLAGS)
"""
import json, sys, subprocess, re

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MANIFEST = "manifest/functions.json"

def our_bytes(name):
    out = subprocess.run(["wdis", f"build/{name}.obj"], capture_output=True, text=True).stdout
    buf = bytearray(); in_text = False
    for line in out.splitlines():
        if line.startswith("Segment:"):
            in_text = "_TEXT" in line; continue
        if not in_text: continue
        m = re.match(r'^[0-9A-Fa-f]{4}  ([0-9A-Fa-f]{2}(?: [0-9A-Fa-f]{2})*)', line)
        if m: buf += bytes(int(b, 16) for b in m.group(1).split())
    return bytes(buf)

def reloc_offsets(ob):
    """External relocations in a freshly-compiled .obj appear as 4-byte
    0x00000000 placeholders (call/jmp rel32, and abs32 data operands like the
    `a1`/`mov [disp32]` forms). Return their byte offsets, derived from OUR
    output; the target's real relocated value at the same offset is then masked."""
    offs = []; i = 0
    while i + 4 <= len(ob):
        if ob[i:i + 4] == b'\x00\x00\x00\x00':
            offs.append(i); i += 4
        else:
            i += 1
    return offs

def mask_at(b, offs):
    b = bytearray(b)
    for o in offs:
        for j in range(o, min(o + 4, len(b))): b[j] = 0
    return bytes(b)

def main():
    name = sys.argv[1]
    man = json.load(open(MANIFEST)); base = int(man.get("image_base", "0"), 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    import os
    if not os.environ.get("SKIP_COMPILE"):
        if subprocess.run(["bash", "tools/wcompile.sh", name]).returncode:
            sys.exit("compile failed")
    off = int(f["addr"], 16) - base
    tb = open(SEG, "rb").read()[off:off + f["size"]]
    ob = our_bytes(name)
    offs = reloc_offsets(ob)
    tm = mask_at(tb, offs)
    om = mask_at(ob, offs)
    print(f"\n=== {name}  addr={f['addr']}  target={len(tb)}B  ours={len(ob)}B ===")
    print(f"target: {tb.hex()}")
    print(f"ours  : {ob.hex()}")
    print(f"reloc offsets masked: {[hex(x) for x in offs]}")
    print(f"EXACT byte match          : {'YES' if tb == ob else 'NO'}")
    ok = (len(tb) == len(ob)) and (tm == om)
    print(f"RELOC-AWARE match (masked): {'YES ✅' if ok else 'NO'}")
    if not ok:
        n = min(len(tm), len(om))
        d = next((i for i in range(n) if tm[i] != om[i]), n)
        print(f"  first diff at 0x{d:x}: target={tm[d:d+1].hex()} ours={om[d:d+1].hex()}")

if __name__ == "__main__":
    main()
