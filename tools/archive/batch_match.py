#!/usr/bin/env python3
"""Sweep all unmatched functions for simple, mechanically-generatable patterns,
compile each with period Watcom 10.0a, and bank relocation-aware matches.
Failed drafts are removed; the manifest is updated in place. Run in container."""
import json, subprocess, struct, os

MAN = "manifest/functions.json"
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
m = json.load(open(MAN)); base = int(m["image_base"], 16)
seg = open(SEG, "rb").read()
RFLAGS = "-4r -oneatx -zp8 -s -zq"

def try_match(name, csrc, flags):
    p = f"src/{name}.c"
    open(p, "w").write(csrc)
    subprocess.run(["bash", "tools/wcc_dos.sh", name, flags], capture_output=True)
    env = {**os.environ, "SKIP_COMPILE": "1"}
    r = subprocess.run(["python3", "tools/match_reloc.py", name],
                       capture_output=True, text=True, env=env)
    ok = "RELOC-AWARE match (masked): YES" in r.stdout
    if not ok:
        os.remove(p)
    return ok

def candidates(f, b):
    n, sz = f["name"], f["size"]
    if sz == 6 and b[0] == 0xB8 and b[5] == 0xC3:              # mov eax,imm32; ret
        imm = struct.unpack_from("<I", b, 1)[0]
        yield (f"/* return constant @ {f['addr']} */\nint {n}(void) {{ return (int)0x{imm:x}; }}\n", RFLAGS)
    if sz == 6 and b[0] == 0xA1 and b[5] == 0xC3:              # mov eax,[addr]; ret
        a = struct.unpack_from("<I", b, 1)[0]
        yield (f"/* global getter @ {f['addr']} */\nextern int g_{a:x};\nint {n}(void) {{ return g_{a:x}; }}\n", RFLAGS)
    if sz == 6 and b[0] == 0xA3 and b[5] == 0xC3:              # mov [addr],eax; ret
        a = struct.unpack_from("<I", b, 1)[0]
        yield (f"/* global setter @ {f['addr']} */\nextern int g_{a:x};\nvoid {n}(int x) {{ g_{a:x} = x; }}\n", RFLAGS)

matched = []
for f in m["functions"]:
    if f["status"] != "unmatched":
        continue
    off = int(f["addr"], 16) - base
    b = seg[off:off + f["size"]]
    if len(b) < f["size"]:
        continue
    for csrc, flags in candidates(f, b):
        if try_match(f["name"], csrc, flags):
            matched.append(f["name"]); print(f"  {f['name']} ({f['size']}B) MATCH ✅"); break

lines = open(MAN).read().split("\n"); mset = set(matched)
for i, ln in enumerate(lines):
    for name in mset:
        if f'"name": "{name}"' in ln and '"status": "unmatched"' in ln:
            lines[i] = ln.replace('"status": "unmatched", "match_pct": 0.0, "src": null',
                                  f'"status": "matched", "match_pct": 100.0, "src": "src/{name}.c"')
open(MAN, "w").write("\n".join(lines))
print(f"\n=== batch_match: matched {len(matched)} functions ===")
