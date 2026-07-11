#!/usr/bin/env python3
"""Regress the existing matches against the 9.5 compiler. For each recipe, compile
with Watcom 9.5b (tools/wcc_95.sh) using the recorded flags and check it still
byte-matches. Tells us whether 9.5 is a viable replacement for 10.0a: if it keeps
everything AND fixes byte-bit-test functions, the game was built with 9.5.
    docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/regress95.py
"""
import json, subprocess

recipes = json.load(open("manifest/recipes.json"))
npass, broke = 0, []
for i, (name, rec) in enumerate(recipes.items()):
    r = subprocess.run(["bash", "tools/match95.sh", name, rec["flags"]],
                       capture_output=True, text=True)
    ok = "RELOC-AWARE match (masked): YES" in r.stdout
    npass += ok
    tag = "ok " if ok else "BROKE"
    print(f"[{i+1}/{len(recipes)}] {tag} {name}  ({rec['flags']})")
    if not ok:
        broke.append(name)
print(f"\n9.5 regression: {npass}/{len(recipes)} still match")
if broke:
    print("broke:", ", ".join(broke))
