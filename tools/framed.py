#!/usr/bin/env python3
"""List UNMATCHED functions whose target bytes start with the frame prologue
55 89 e5 (push ebp; mov ebp,esp) -- the framed class. Print size + first bytes
so forwarder/wrapper shapes can be spotted and banked with `-3s -of`."""
import json
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
man = json.load(open("manifest/functions.json"))
base = int(man["image_base"], 16)
data = open(SEG, "rb").read()
rows = []
for f in man["functions"]:
    if f["status"] == "matched":
        continue
    off = int(f["addr"], 16) - base
    b = data[off:off + min(f["size"], 28)]
    if b[:3] == b"\x55\x89\xe5":
        rows.append((f["size"], f["addr"], f["name"], b.hex()))
rows.sort()
print(f"framed unmatched: {len(rows)}")
for sz, addr, name, hx in rows:
    print(f"  {addr} sz={sz:4d} {name:16s} {hx}")
