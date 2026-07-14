#!/usr/bin/env python3
"""Mark a function matched in manifest/functions.json (targeted, format-preserving).
  python3 tools/mark.py <name> [src_path]"""
import sys, glob
name = sys.argv[1]
if len(sys.argv) > 2:
    src = sys.argv[2]
else:                                    # src files live in subsystem subdirs; find by name
    _h = glob.glob(f"src/**/{name}.c", recursive=True)
    src = _h[0] if _h else f"src/unclassified/{name}.c"
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
