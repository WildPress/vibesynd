#!/usr/bin/env python3
"""Alignment-aware diff: truediff with SequenceMatcher opcodes.
Usage: seqdiff.py <name> <true_size> <codestart>"""
import json, sys, os
from difflib import SequenceMatcher
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from match_reloc import text_bytes_and_fixups, mask, MANIFEST, SEG

name, tsize, codestart = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0)
man = json.load(open(MANIFEST)); base = int(man.get("image_base", "0"), 16)
f = next(x for x in man["functions"] if x["name"] == name)
off = int(f["addr"], 16) - base
tb = open(SEG, "rb").read()[off:off + tsize]
ob, fixups = text_bytes_and_fixups(f"build/{name}.obj")
code = ob[codestart:]
cfx = [(o - codestart, s) for o, s in fixups if o >= codestart]
# mask fixups in ours; mask the SAME target spans is wrong when lengths drift,
# so mask target fixup-looking spans by masking ours and zeroing target at ours' spans
# via alignment: instead, zero 4-byte spans in ours at cfx, and in target zero
# spans at cfx offsets only when lengths equal. For drifted compare, just zero ours' fixups
# and let target's absolute addresses show as diffs (they are 4B dword operands).
cm = mask(code, cfx)
sm = SequenceMatcher(None, tb, cm, autojunk=False)
print(f"target {len(tb)}B  ours-code {len(cm)}B  ratio {sm.ratio():.4f}")
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == "equal":
        continue
    print(f"{tag:8s} T[0x{i1:x}:0x{i2:x}] {tb[i1:i2].hex()}  |  O[0x{j1:x}:0x{j2:x}] {cm[j1:j2].hex()}")
