#!/usr/bin/env python3
"""Flag calibration against the regression baseline (manifest/recipes.json).

Substitutes a candidate optimisation bundle and/or packing into every recorded
recipe (keeping each function's calling convention untouched), recompiles, and
reports how many still match. A candidate that breaks ANY existing match is ruled
out. A candidate that keeps 100% is still viable and worth testing on new/complex
functions.

    docker run --rm -v "$PWD":/work -w /work synd-decomp \
        python3 tools/caltest.py --opt -ox --pack -zp8

Defaults reproduce the current baseline (should be 100%).
"""
import json, subprocess, sys

def parse(argv):
    opt, pack = "-oneatx", "-zp8"
    i = 1
    while i < len(argv):
        if argv[i] == "--opt": opt = argv[i+1]; i += 2
        elif argv[i] == "--pack": pack = argv[i+1]; i += 2
        else: i += 1
    return opt, pack

def sub_flags(flags, opt, pack):
    toks = flags.split()
    out = []
    for t in toks:
        if t == "-oneatx": out.append(opt)
        elif t.startswith("-zp"): out.append(pack)
        else: out.append(t)
    return " ".join(out)

def try_match(name, flags):
    r = subprocess.run(["bash", "tools/match10.sh", name, flags],
                       capture_output=True, text=True)
    return "RELOC-AWARE match (masked): YES" in r.stdout

def main():
    opt, pack = parse(sys.argv)
    recipes = json.load(open("manifest/recipes.json"))
    npass, broke = 0, []
    for name, rec in recipes.items():
        fl = sub_flags(rec["flags"], opt, pack)
        if try_match(name, fl):
            npass += 1
        else:
            broke.append(name)
    n = len(recipes)
    print(f"candidate: opt={opt} pack={pack}")
    print(f"regression: {npass}/{n} still match" + ("  (VIABLE)" if npass == n else "  (RULED OUT)"))
    if broke:
        print("broke:", ", ".join(broke[:20]) + (" ..." if len(broke) > 20 else ""))

if __name__ == "__main__":
    main()
