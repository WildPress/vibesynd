#!/usr/bin/env python3
"""Diff a compiled fn against the REAL segment bytes with a TRUE-SIZE override
(manifest untouched). Usage: truediff.py <name> <true_size> [ctx]"""
import json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from match_reloc import text_bytes_and_fixups, mask, leading_jumptable_entries, MANIFEST, SEG

name, tsize = sys.argv[1], int(sys.argv[2], 0)
ctx = int(sys.argv[3]) if len(sys.argv) > 3 else 16
man = json.load(open(MANIFEST)); base = int(man.get("image_base", "0"), 16)
f = next(x for x in man["functions"] if x["name"] == name)
off = int(f["addr"], 16) - base
tb = open(SEG, "rb").read()[off:off + tsize]
ob, fixups = text_bytes_and_fixups(f"build/{name}.obj")
K = leading_jumptable_entries(fixups)
codestart = len(ob) - tsize
print(f"table entries={K} obj={len(ob)}B target(true)={tsize}B codestart={codestart} (table {4*K}B + pad {codestart-4*K}B)")
if codestart < 0:
    codestart = 0
code = ob[codestart:]
cfx = [(o - codestart, s) for o, s in fixups if o >= codestart]
tm, cm = mask(tb, cfx), mask(code, cfx)
if tm == cm and len(tb) == len(code):
    print("TRUE-SIZE MASKED MATCH: YES")
    sys.exit(0)
print(f"lengths: target={len(tb)} ours-code={len(code)} (delta {len(code)-len(tb):+d})")
n = min(len(tm), len(cm))
same = sum(1 for i in range(n) if tm[i] == cm[i])
print(f"positional same bytes: {same}/{n}")
i = 0; shown = 0
while i < n and shown < 12:
    if tm[i] != cm[i]:
        s = max(0, i - ctx); e = min(n, i + ctx)
        print(f"diff @0x{i:x}:\n  T {tm[s:e].hex()}\n  O {cm[s:e].hex()}")
        shown += 1
        i = e
    else:
        i += 1
