#!/usr/bin/env python3
"""List UNMATCHED, frameless GAME-code functions (addr < 0x3a000, i.e. below the
runtime-library region), sorted by size, with first bytes. These are the real
decompilation targets. Usage: nextgame.py [maxsize] [count]"""
import json, sys
maxsize = int(sys.argv[1]) if len(sys.argv) > 1 else 80
n = int(sys.argv[2]) if len(sys.argv) > 2 else 30
man = json.load(open("manifest/functions.json")); base = int(man["image_base"], 16)
data = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
rows = []
for f in man["functions"]:
    if f["status"] == "matched":
        continue
    a = int(f["addr"], 16)
    if a >= 0x3a000:            # skip the linked-in runtime library region
        continue
    if f["size"] > maxsize or f["size"] < 4:
        continue
    off = a - base
    b = data[off:off + min(f["size"], 24)]
    if b[:3] == b"\x55\x89\xe5":   # skip framed (blocked / library-style)
        continue
    rows.append((f["size"], f["addr"], f["calls"], f["data_refs"], b.hex()))
rows.sort()
print(f"frameless game-code unmatched (size<= {maxsize}): {len(rows)} shown {min(n,len(rows))}")
for sz, addr, calls, drefs, hx in rows[:n]:
    print(f"  {addr} sz={sz:4d} calls={calls} data={drefs} {hx}")
