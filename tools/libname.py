#!/usr/bin/env python3
"""Name the library functions: for each framed (0x3a000+) function, find its bytes
inside the Watcom CLIB .LIB files and report the containing MODULE name.

Uses `wlib <lib>` to get each module's file offset, then finds the function's
distinctive byte window in the .LIB and maps its position to the module whose
[offset, next_offset) range contains it. Prints addr, size, coverage, lib:module.
Functions with no library hit are the GAME's own code."""
import json, subprocess, re, os

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
# The 9.5 .LIB files are decompressed from the original floppy media with
# tools/archive/wunpack95.sh (WPACK under DOSBox). CLIB3S/R is the C runtime;
# GRAPH/MATH were added to TEST whether the game links Watcom's graphics or math
# libraries -- it does NOT (lib/gfx and lib/sound score 0%, they are the game's
# own code, proven not-Watcom). The 10.0a CLIBs are kept only for contrast; the
# game matches the 9.5 build, so it scores ~0% against them.
LIBS = [
    ("95S", "toolchain/watcom95/unpacked/CLIB3S.LIB"),
    ("95R", "toolchain/watcom95/unpacked/CLIB3R.LIB"),
    ("GR",  "toolchain/watcom95/unpacked/GRAPH.LIB"),
    ("M387","toolchain/watcom95/unpacked/MATH387S.LIB"),
    ("M3",  "toolchain/watcom95/unpacked/MATH3S.LIB"),
    ("10S", "toolchain/watcom10a/WATCOM/LIB386/DOS/CLIB3S.LIB"),
    ("10R", "toolchain/watcom10a/WATCOM/LIB386/DOS/CLIB3R.LIB"),
]
W = 12

def load_lib(path):
    data = open(path, "rb").read()
    out = subprocess.run(["wlib", path], capture_output=True, text=True).stdout
    mods = []
    for m in re.finditer(r'^(\S+) Offset=([0-9A-Fa-f]+)H', out, re.M):
        mods.append((int(m.group(2), 16), m.group(1)))
    mods.sort()
    return data, mods

def module_at(mods, pos):
    name = None
    for off, nm in mods:
        if off <= pos:
            name = nm
        else:
            break
    return name

libs = [(tag, *load_lib(p)) for tag, p in LIBS if os.path.exists(p)]
man = json.load(open("manifest/functions.json"))
base = int(man["image_base"], 16)
seg = open(SEG, "rb").read()

rows = []
for f in man["functions"]:
    off = int(f["addr"], 16) - base
    b = seg[off:off + f["size"]]
    if not (b[:3] == b"\x55\x89\xe5" or b[:4] == b"\x53\x55\x89\xe5"):
        continue
    wins = [b[i:i + W] for i in range(len(b) - W + 1)]
    best = None
    if wins:
        for tag, data, mods in libs:
            hits = sum(1 for w in wins if w in data)
            cov = hits / len(wins)
            if best is None or cov > best[0]:
                # name from the position of the LONGEST contiguous run of b in data
                nm, bestlen = None, 0
                for i in range(len(b)):
                    for j in range(len(b), i + bestlen, -1):
                        p = data.find(b[i:j])
                        if p != -1:
                            if j - i > bestlen:
                                bestlen = j - i; nm = module_at(mods, p)
                            break
                best = (cov, tag, nm)
    cov, tag, nm = best if best else (0, "-", None)
    rows.append((cov, f["addr"], f["size"], f["status"], f["name"], tag, nm))

rows.sort(reverse=True)
print(f"{'cov':>5}  addr      size  status      FUN               lib:module")
out = []
for cov, addr, sz, st, name, tag, nm in rows:
    if cov >= 0.40 and nm:
        cls, lab = "library", f"{tag}:{nm}"
    elif cov >= 0.15:
        cls, lab = "library?", f"~{tag}:{nm}"
    else:
        cls, lab = "game", "GAME (not in lib)"
    print(f"{cov*100:4.0f}%  {addr}  {sz:4d}  {st:9s}  {name:16s}  {lab}")
    out.append({"addr": addr, "fun": name, "size": sz, "status": st,
                "coverage": round(cov, 3), "class": cls,
                "lib": tag if cls != "game" else None,
                "module": nm if cls != "game" else None})
if os.environ.get("WRITE_JSON"):
    json.dump(out, open("manifest/library_functions.json", "w"), indent=1)
    print(f"\nwrote manifest/library_functions.json ({len(out)} entries)")
