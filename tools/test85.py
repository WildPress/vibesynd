#!/usr/bin/env python3
"""Test Watcom 8.5a against known-matched fns (regression) + walls. Compile each with 8.5a,
reloc-mask vs target, report. If 8.5 misses the ALREADY-matched fns, it's not the game's compiler."""
import sys, subprocess, json, os
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
man = json.load(open("manifest/functions.json"))
base = int(man["image_base"], 16)
byname = {f["name"]: f for f in man["functions"]}
IMG = open(SEG, "rb").read()

def mask(b, fx):
    b = bytearray(b)
    for o, s in fx:
        for j in range(o, min(o + s, len(b))): b[j] = 0
    return bytes(b)

def compile85(name, flags):
    subprocess.run(["rm", "-f", f"build/{name}.obj"])
    r = subprocess.run(["bash", "tools/wcc_85.sh", name, flags], capture_output=True, text=True)
    if not os.path.exists(f"build/{name}.obj"):
        return None, (r.stdout + r.stderr)[-200:]
    return text_bytes_and_fixups(f"build/{name}.obj"), None

def main():
    names = sys.argv[1:] or ["FUN_0000e568", "FUN_0000fee8", "FUN_00034048", "FUN_00034088"]
    flags = os.environ.get("F85", "-4s -oneatx -zp8 -s -zq")
    print(f"Watcom 8.5a test, flags: {flags}\n")
    for name in names:
        f = byname.get(name)
        if not f:
            print(f"{name}: not in manifest"); continue
        addr, size, status = int(f["addr"], 16), f["size"], f.get("status")
        res, err = compile85(name, flags)
        if res is None:
            print(f"{name:16s} [{status:9s}] COMPILE-FAIL: {err.strip()[:80]}"); continue
        ob, fx = res
        tb = IMG[addr - base: addr - base + size]
        ok = len(ob) == size and mask(tb, fx) == mask(ob, fx)
        tag = "MATCH ✅" if ok else f"no ({len(ob)}B vs {size}B)"
        print(f"{name:16s} [{status:9s}] 8.5a -> {tag}")
    print("\n(matched fns that 8.5a MISSES => 8.5 is not the game's compiler)")

if __name__ == "__main__":
    main()
