#!/usr/bin/env python3
"""Classify every matched function by which OPTIMISATION level it actually needs,
holding its calling convention fixed. For each recipe we strip the optimisation
and packing tokens, then recompile (Watcom 9.5) under each candidate opt and note
which produce a byte-identical (reloc-masked) match.

The point: decide whether the binary is one optimisation level with per-function
C, or genuinely several translation units. If the results partition cleanly and
cluster by address, they're real units; if they scatter, something's wrong.

    docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/optclass.py
"""
import json, subprocess

OPT_TOKENS = {"-oneatx", "-or", "-ot", "-os", "-ox", "-oi", "-oa", "-oat", "-orat"}
CANDIDATES = [
    ("oneatx", "-oneatx -zp8"),
    ("ot",     "-ot"),
    ("or",     "-or"),
]

def cc_of(flags):
    """calling-convention / structural tokens only (drop opt + packing)."""
    return [t for t in flags.split()
            if t not in OPT_TOKENS and not t.startswith("-zp")]

def try_match(name, flags):
    r = subprocess.run(["bash", "tools/match95.sh", name, flags],
                       capture_output=True, text=True)
    return "RELOC-AWARE match (masked): YES" in r.stdout

def main():
    recipes = json.load(open("manifest/recipes.json"))
    rows = []
    for name, rec in recipes.items():
        cc = cc_of(rec["flags"])
        # cc still carries -s -zq (kept); build "<cc-minus-tail> <opt> -s -zq"
        tail = [t for t in cc if t in ("-s", "-zq")]
        head = [t for t in cc if t not in ("-s", "-zq")]
        res = {}
        for label, opt in CANDIDATES:
            flags = " ".join(head + opt.split() + tail)
            res[label] = try_match(name, flags)
        rows.append((int(rec["addr"], 16), name, " ".join(head), res))
    rows.sort()
    print(f"{'addr':>8}  {'conv':<9} {'oneatx':>6} {'ot':>3} {'or':>3}  needs")
    for addr, name, conv, res in rows:
        flags = "".join("Y" if res[k] else "." for k in ("oneatx", "ot", "or"))
        only_oneatx = res["oneatx"] and not (res["ot"] or res["or"])
        only_light  = (res["ot"] or res["or"]) and not res["oneatx"]
        need = "ONEATX-only" if only_oneatx else ("LIGHT-only" if only_light else
               ("both" if res["oneatx"] else "NEITHER"))
        print(f"{addr:08x}  {conv:<9} "
              f"{'Y' if res['oneatx'] else '.':>6} {'Y' if res['ot'] else '.':>3} "
              f"{'Y' if res['or'] else '.':>3}  {need}")

if __name__ == "__main__":
    main()
