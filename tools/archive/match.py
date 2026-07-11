#!/usr/bin/env python3
"""Per-function match loop (runs inside the synd-decomp container).

  python3 tools/match.py <function-name>

Reads the function from manifest/functions.json, extracts the target machine-code
bytes from the code segment (base 0x0 => addr == file offset), compiles
src/<name>.c with Open Watcom, extracts our compiled bytes from the .obj (via
wdis), and reports a byte-level match plus side-by-side disassembly.

The oracle is byte equality. Relocation-aware diffing (ignoring call/data target
addresses) will layer on top once we tackle non-leaf targets with objdiff.
"""
import json, sys, subprocess, re

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MANIFEST = "manifest/functions.json"


def load_manifest():
    return json.load(open(MANIFEST))


def find_fn(man, name):
    for f in man["functions"]:
        if f["name"] == name:
            return f
    sys.exit(f"function {name!r} not in manifest")


def target_bytes(f, base):
    off = int(f["addr"], 16) - base          # addresses are image-base relative
    return open(SEG, "rb").read()[off:off + f["size"]]


def compile_unit(name):
    r = subprocess.run(["bash", "tools/wcompile.sh", name])
    if r.returncode != 0:
        sys.exit("compile failed")


def our_bytes(name):
    out = subprocess.run(["wdis", f"build/{name}.obj"],
                         capture_output=True, text=True).stdout
    buf = bytearray()
    in_text = False
    for line in out.splitlines():
        if line.startswith("Segment:"):
            in_text = "_TEXT" in line
            continue
        if not in_text:
            continue
        # instruction line: 'AAAA  bb bb bb   <tab> mnemonic'
        m = re.match(r'^[0-9A-Fa-f]{4}  ([0-9A-Fa-f]{2}(?: [0-9A-Fa-f]{2})*)', line)
        if m:
            buf += bytes(int(b, 16) for b in m.group(1).split())
    return bytes(buf)


def disasm(b):
    open("/tmp/d.bin", "wb").write(b)
    out = subprocess.run(
        ["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel", "/tmp/d.bin"],
        capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        mm = re.match(r'^\s*([0-9a-f]+):\t([0-9a-f ]+?)\t(.*)$', line)
        if mm:
            rows.append((int(mm.group(1), 16), mm.group(2).strip(), mm.group(3).strip()))
    return rows


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: match.py <function-name>")
    name = sys.argv[1]
    man = load_manifest()
    base = int(man.get("image_base", "0"), 16)
    f = find_fn(man, name)
    compile_unit(name)
    tb = target_bytes(f, base)
    ob = our_bytes(name)

    print(f"\n=== {name}  addr={f['addr']} target_size={len(tb)} our_size={len(ob)} ===")
    same = sum(1 for i in range(min(len(tb), ob and len(ob) or 0)) if tb[i] == ob[i])
    denom = max(len(tb), len(ob)) or 1
    pct = 100.0 * same / denom
    match = (tb == ob)
    print(f"BYTE MATCH: {'YES ✅' if match else 'NO'}   ({same}/{denom} bytes, {pct:.1f}%)")
    if not match:
        first = next((i for i in range(min(len(tb), len(ob))) if tb[i] != ob[i]),
                     min(len(tb), len(ob)))
        print(f"first difference at offset 0x{first:x}")
    print(f"target: {tb.hex()}")
    print(f"ours  : {ob.hex()}")

    tdis, odis = disasm(tb), disasm(ob)
    print(f"\n{'TARGET (original)':42} | OURS (Watcom)")
    print("-" * 90)
    for i in range(max(len(tdis), len(odis))):
        t = f"{tdis[i][0]:04x}  {tdis[i][1]:<16} {tdis[i][2]}" if i < len(tdis) else ""
        o = f"{odis[i][0]:04x}  {odis[i][1]:<16} {odis[i][2]}" if i < len(odis) else ""
        flag = "" if t[6:] == o[6:] else "  <-- diff"
        print(f"{t:42} | {o}{flag}")


if __name__ == "__main__":
    main()
