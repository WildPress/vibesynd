#!/usr/bin/env python3
"""One-off true-size diff for FUN_00011d68 (manifest size is wrong: true 0xb41).
Mimics match_reloc.py incl. the jump-table-aware tail compare. No manifest edit."""
import sys
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
ADDR = 0x11d68
SIZE = 0xb41

def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)

tb = open(SEG, "rb").read()[ADDR - 0x10000: ADDR - 0x10000 + SIZE]
ob, fixups = text_bytes_and_fixups("build/FUN_00011d68.obj")
print(f"target={len(tb)}B ours={len(ob)}B (obj may lead with jump tables)")

# find leading run of 4-byte fixups (jump tables co-located before code)
offs = {o: s for o, s in fixups}
k = 0
while offs.get(4 * k) == 4:
    k += 1
print(f"leading table entries: {k} ({4*k} bytes)")
codestart = len(ob) - len(tb)
if codestart < 0:
    codestart = 0
code = ob[codestart:]
cfx = [(o - codestart, s) for o, s in fixups if o >= codestart]
tm, om = mask(tb, cfx), mask(code, cfx)
n = min(len(tm), len(om))
match = sum(1 for i in range(n) if tm[i] == om[i])
d = next((i for i in range(n) if tm[i] != om[i]), n)
print(f"code tail compare: {len(code)}B vs {len(tb)}B; bytes equal at same offset: {match}/{len(tb)}")
print(f"first diff at 0x{d:x}: target={tm[d:d+8].hex()} ours={om[d:d+8].hex()}")
# show a few context windows around early diffs
shown = 0
i = 0
while i < n and shown < 8:
    if tm[i] != om[i]:
        j = i
        while j < n and (tm[j] != om[j] or (j+4 < n and any(tm[j+x] != om[j+x] for x in range(4)))):
            j += 1
        print(f"  diff 0x{i:x}..0x{j:x}: T={tm[i:min(j,i+24)].hex()} O={om[i:min(j,i+24)].hex()}")
        shown += 1
        i = j + 1
    else:
        i += 1
