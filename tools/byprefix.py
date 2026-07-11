#!/usr/bin/env python3
"""List UNMATCHED functions whose target bytes start with a given hex prefix,
sorted by size, with first bytes. Usage: byprefix.py <hexbyte> [count]"""
import json, sys
pre = bytes.fromhex(sys.argv[1]) if len(sys.argv) > 1 else b"\x53"
n = int(sys.argv[2]) if len(sys.argv) > 2 else 25
man = json.load(open("manifest/functions.json")); base = int(man["image_base"], 16)
data = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
rows = []
for f in man["functions"]:
    if f["status"] == "matched":
        continue
    off = int(f["addr"], 16) - base
    b = data[off:off + min(f["size"], 26)]
    if b[:len(pre)] == pre:
        rows.append((f["size"], f["addr"], f["name"], f["calls"], b.hex()))
rows.sort()
print(f"prefix {pre.hex()} unmatched: {len(rows)}  (showing {min(n,len(rows))})")
for sz, addr, name, calls, hx in rows[:n]:
    print(f"  {addr} sz={sz:4d} calls={calls} {hx}")
