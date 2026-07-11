#!/usr/bin/env python3
"""Batch-match the dispatch-stub cluster: 10-byte functions of the form
`mov eax,<imm32> ; jmp <rel32>` (opcodes B8 .. E9 ..). Each is generated as
`return <handler>(imm)`, compiled with period Watcom 10.0a (-5r, tail-call ->
jmp), and byte-matched relocation-aware (the jmp target is masked).
Updates the manifest in place for matches. Run in the container."""
import json, subprocess, struct, os

MAN = "manifest/functions.json"
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
m = json.load(open(MAN))
base = int(m["image_base"], 16)
seg = open(SEG, "rb").read()

def do(name, flags):
    subprocess.run(["bash", "tools/wcc_dos.sh", name, flags], capture_output=True)
    env = {**os.environ, "SKIP_COMPILE": "1"}
    r = subprocess.run(["python3", "tools/match_reloc.py", name],
                       capture_output=True, text=True, env=env)
    return "RELOC-AWARE match (masked): YES" in r.stdout

matched = []
for f in m["functions"]:
    if f["status"] != "unmatched" or f["size"] != 10:
        continue
    off = int(f["addr"], 16) - base
    b = seg[off:off + 10]
    if len(b) < 10 or b[0] != 0xB8 or b[5] != 0xE9:
        continue
    imm = struct.unpack_from("<I", b, 1)[0]
    rel = struct.unpack_from("<i", b, 6)[0]
    tgt = (int(f["addr"], 16) + 10 + rel) & 0xFFFFFFFF
    h = f"FUN_{tgt:08x}"
    name = f["name"]
    open(f"src/{name}.c", "w").write(
        f"/* dispatch stub @ {f['addr']} ({f['size']}B): mov eax,0x{imm:x}; jmp {h} (tail call) */\n"
        f"extern int {h}(int);\n"
        f"int {name}(void) {{ return {h}(0x{imm:x}); }}\n")
    ok = do(name, "-5r -oneatx -zp8 -s -zq")
    print(f"  {name}  imm=0x{imm:<4x} -> {h}  {'MATCH ✅' if ok else 'no'}")
    if ok:
        matched.append(name)

# update manifest in place, preserving one-line-per-function formatting
lines = open(MAN).read().split("\n")
mset = set(matched)
for i, ln in enumerate(lines):
    for name in mset:
        if f'"name": "{name}"' in ln and '"status": "unmatched"' in ln:
            lines[i] = ln.replace('"status": "unmatched", "match_pct": 0.0, "src": null',
                                  f'"status": "matched", "match_pct": 100.0, "src": "src/{name}.c"')
open(MAN, "w").write("\n".join(lines))
print(f"\n=== dispatch cluster: matched {len(matched)} functions; manifest updated ===")
