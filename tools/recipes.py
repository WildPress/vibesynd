#!/usr/bin/env python3
"""Derive and record the compile flags for every matched function -> a regression
baseline. For each matched function, try candidate flag sets in order and record the
first that gives a reloc-aware match. Writes manifest/recipes.json:
    { "FUN_xxx": {"addr": "...", "flags": "..."} , ... }
Any function that matches NONE is reported (its src may have drifted, or the
candidate list is incomplete). Run in-container:
    docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/recipes.py
"""
import json, subprocess, sys, os

# most game funcs are -4s; register-calling -4r; runtime-library framed -3s -of.
CANDIDATES = [
    "-4s -oneatx -zp8 -s -zq",
    "-4r -oneatx -zp8 -s -zq",
    "-3s -of -oneatx -zp8 -s -zq",
    "-3r -of -oneatx -zp8 -s -zq",
    "-3s -oneatx -zp8 -s -zq",
    "-4s -of -oneatx -zp8 -s -zq",
    "-4s -or -zp8 -s -zq",
    "-4s -ot -s -zq",
    "-3s -d2 -oneatx -zp8 -s -zq",
    "-3s -os -zp8 -s -zq",
    "-3s -os -ol -zp8 -s -zq",
]

def try_match(name, flags):
    r = subprocess.run(["bash", "tools/match95.sh", name, flags],
                       capture_output=True, text=True)
    return ("RELOC-AWARE match (masked): YES" in r.stdout
            or "JUMP-TABLE-AWARE match    : YES" in r.stdout)

def main():
    man = json.load(open("manifest/functions.json"))
    matched = [f for f in man["functions"] if f["status"] == "matched" and f.get("src")]
    recipes, misses = {}, []
    for i, f in enumerate(matched):
        name = f["name"]
        hit = None
        for fl in CANDIDATES:
            if try_match(name, fl):
                hit = fl; break
        if hit:
            recipes[name] = {"addr": f["addr"], "flags": hit}
            print(f"[{i+1}/{len(matched)}] {name}: {hit}")
        else:
            misses.append(name)
            print(f"[{i+1}/{len(matched)}] {name}: NO MATCH (any candidate)")
    json.dump(recipes, open("manifest/recipes.json", "w"), indent=1)
    print(f"\nwrote manifest/recipes.json: {len(recipes)} recorded, {len(misses)} misses")
    if misses:
        print("misses:", ", ".join(misses))

if __name__ == "__main__":
    main()
