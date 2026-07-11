#!/usr/bin/env python3
"""Mark a function matched in manifest/functions.json (targeted, format-preserving).
  python3 tools/mark.py <name> [src_path]"""
import sys
name = sys.argv[1]
src = sys.argv[2] if len(sys.argv) > 2 else f"src/{name}.c"
MAN = "manifest/functions.json"
lines = open(MAN).read().split("\n")
done = False
for i, ln in enumerate(lines):
    if f'"name": "{name}"' in ln and '"status": "unmatched"' in ln:
        lines[i] = ln.replace('"status": "unmatched", "match_pct": 0.0, "src": null',
                              f'"status": "matched", "match_pct": 100.0, "src": "{src}"')
        done = True
open(MAN, "w").write("\n".join(lines))
print(f"marked {name}" if done else f"{name}: not found / already matched")
