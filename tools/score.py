#!/usr/bin/env python3
"""Completion score for the matching decompilation.

Byte coverage is the honest headline: matched machine-code bytes over total.
Function count is shown too, but it flatters progress because early matches are
tiny. Runtime-library functions (see manifest/library_functions.json) are split
out, since they'll be linked from the original library rather than decompiled, so
the real target is the game's own code.

Usage: python3 tools/score.py [--json]
"""
import json, sys

man = json.load(open("manifest/functions.json"))
fns = man["functions"]
try:
    lib = {e["addr"] for e in json.load(open("manifest/library_functions.json"))
           if e.get("class") == "library"}
except FileNotFoundError:
    lib = set()

def agg(sel):
    sub = [f for f in fns if sel(f)]
    n = len(sub)
    b = sum(f["size"] for f in sub)
    m = [f for f in sub if f["status"] == "matched"]
    mn = len(m)
    mb = sum(f["size"] for f in m)
    return n, b, mn, mb

def pct(a, b):
    return (100.0 * a / b) if b else 0.0

# overall
N, B, MN, MB = agg(lambda f: True)
# library vs game
LN, LB, LMN, LMB = agg(lambda f: f["addr"] in lib)
GN, GB, GMN, GMB = agg(lambda f: f["addr"] not in lib)

if "--badge-url" in sys.argv:
    # static shields.io badge URL (works on private repos, unlike the endpoint kind).
    import urllib.parse
    msg = urllib.parse.quote(f"{pct(GMB, GB):.2f}% bytes | {GMN}/{GN} fns", safe="")
    print(f"https://img.shields.io/badge/game%20matched-{msg}-blue")
    sys.exit(0)

if "--json" in sys.argv:
    print(json.dumps({
        "overall": {"functions": N, "matched_functions": MN, "bytes": B, "matched_bytes": MB,
                    "fn_pct": round(pct(MN, N), 2), "byte_pct": round(pct(MB, B), 2)},
        "game": {"functions": GN, "matched_functions": GMN, "bytes": GB, "matched_bytes": GMB,
                 "fn_pct": round(pct(GMN, GN), 2), "byte_pct": round(pct(GMB, GB), 2)},
        "library": {"functions": LN, "bytes": LB, "matched_functions": LMN},
    }, indent=1))
    sys.exit(0)

bar = lambda p: "#" * int(p / 5) + "." * (20 - int(p / 5))
print("Syndicate matching decompilation, completion score")
print("=" * 52)
print(f"OVERALL   functions {MN:>3}/{N:<3} ({pct(MN,N):5.1f}%)   bytes {MB:>6}/{B:<6} ({pct(MB,B):5.2f}%)")
print(f"GAME code functions {GMN:>3}/{GN:<3} ({pct(GMN,GN):5.1f}%)   bytes {GMB:>6}/{GB:<6} ({pct(GMB,GB):5.2f}%)")
print(f"          [{bar(pct(GMB,GB))}]  <- game byte coverage (the headline)")
print(f"LIBRARY   {LN} functions ({LB} bytes) identified, to be linked not decompiled")
