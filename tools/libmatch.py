#!/usr/bin/env python3
"""Identify which framed (0x3a000+) functions are LIBRARY code by searching the
Watcom runtime .LIB files for each function's actual bytes.

For every function whose target bytes start with a frame prologue (55 89 e5) OR
push-ebx+frame (53 55 89 e5), slide a 12-byte window across its body and count
how many windows appear verbatim anywhere in the concatenated library blob.
A high hit ratio => the function's compiled code lives in the runtime library.
Relocated bytes (call/jmp/abs operands) won't match, so even a true library fn
won't score 100%; ~>40% window coverage is a strong "this is library" signal."""
import json, glob, os

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
LIBS = [
    "toolchain/watcom95/unpacked/CLIB3R.LIB",
    "toolchain/watcom95/unpacked/CLIB3S.LIB",
    "toolchain/watcom10a/WATCOM/LIB386/DOS/CLIB3R.LIB",
    "toolchain/watcom10a/WATCOM/LIB386/DOS/CLIB3S.LIB",
]
blob = b""
for p in LIBS:
    if os.path.exists(p):
        blob += b"\xff\xff" + open(p, "rb").read()

# index all 12-byte windows of the library blob for fast lookup
W = 12
libset = set()
for i in range(len(blob) - W):
    libset.add(blob[i:i + W])

man = json.load(open("manifest/functions.json"))
base = int(man["image_base"], 16)
data = open(SEG, "rb").read()

rows = []
for f in man["functions"]:
    off = int(f["addr"], 16) - base
    b = data[off:off + f["size"]]
    if not (b[:3] == b"\x55\x89\xe5" or b[:4] == b"\x53\x55\x89\xe5"):
        continue
    wins = [b[i:i + W] for i in range(len(b) - W + 1)]
    if not wins:
        continue
    hit = sum(1 for w in wins if w in libset)
    rows.append((hit / len(wins), f["addr"], f["name"], f["size"], f["status"]))

rows.sort(reverse=True)
print(f"lib blob: {len(blob)} bytes, {len(libset)} distinct {W}-byte windows")
print(f"{'coverage':>9}  addr      size  status      name")
for cov, addr, name, sz, st in rows:
    flag = "  <-- LIBRARY" if cov >= 0.40 else ("  ~partial" if cov >= 0.15 else "")
    print(f"{cov*100:8.1f}%  {addr}  {sz:4d}  {st:10s}  {name}{flag}")
