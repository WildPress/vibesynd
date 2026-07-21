#!/usr/bin/env python3
"""asm_emit_all.py -- emit ALL code functions as relocatable .s in one efficient pass.

Loads every type-7 fixup once (lefix) and disassembles OBJECT1 once (objdump), then
symbolizes each function (same rules as asm_symbolize.py: data refs -> __obj<N> + off,
inter-function calls -> symbolic). Emits one .s per function into port/gen/asm/ plus a
manifest of every external symbol referenced (the callees not defined here + the CLIB/
hardware boundary), which is the precise shim surface for the native link.

Functions in the cut-off prefix (<0x10000, whose bytes are not in OBJECT1.linear.bin)
are skipped and listed.

Usage: python tools/asm_emit_all.py [outdir]
"""
import os, re, sys, json, subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ1 = os.path.join(ROOT, "inputs", "SYNDICAT_MAIN_OBJECT1.linear.bin")
CODE_BASE = 0x10000
IS_WIN = sys.platform == "win32"

def sh(cmd):
    argv = ["wsl", "bash", "-c", cmd] if IS_WIN else ["bash", "-c", cmd]
    return subprocess.run(argv, capture_output=True, text=True)

def envpath(p):
    if not IS_WIN:
        return p
    return subprocess.run(["wsl", "wslpath", "-a", p.replace("\\", "/")],
                          capture_output=True, text=True).stdout.strip()

def all_fixups():
    """{source_off: (objn, toff, add)} for every type-7 code fixup."""
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "lefix.py"),
                          "src", "%x" % CODE_BASE, "0x40000"], capture_output=True, text=True).stdout
    fx = {}
    for line in out.splitlines():
        m = re.match(r"src=0x([0-9a-fA-F]+)\s+stype=7\s+ttype=\d+\s+->\s+obj(\d):\+0x([0-9a-fA-F]+)\s+add=(\S+)", line)
        if m:
            fx[int(m.group(1), 16)] = (int(m.group(2)), int(m.group(3), 16),
                                       None if m.group(4) == "None" else int(m.group(4), 0))
    return fx

def disasm_all():
    """{instr_addr: (raw_bytes_list, rel32_operand_index_or_None)} for the whole image."""
    img = open(OBJ1, "rb").read()
    end = CODE_BASE + len(img)
    cmd = ("objdump -D -b binary -m i386 -M intel --adjust-vma=0x%x "
           "--start-address=0x%x --stop-address=0x%x '%s'"
           % (CODE_BASE, CODE_BASE, end, envpath(OBJ1)))
    txt = sh(cmd).stdout
    insns = {}
    for line in txt.splitlines():
        m = re.match(r"\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2} )+)\s+(\S+)", line)
        if not m:
            continue
        ia = int(m.group(1), 16)
        raw = [int(b, 16) for b in m.group(2).split()]
        opi = None
        if raw and (raw[0] == 0xE8 or raw[0] == 0xE9):
            opi = 1
        elif len(raw) >= 2 and raw[0] == 0x0F and 0x80 <= raw[1] <= 0x8F:
            opi = 2
        insns[ia] = (raw, opi)
    return img, insns

def func_names():
    m = {}
    for f in json.load(open(os.path.join(ROOT, "manifest", "functions.json")))["functions"]:
        try:
            m[int(f["addr"], 16)] = f["name"]
        except (KeyError, ValueError):
            pass
    return m

def emit_one(name, addr, size, img, insns, fixups, names):
    data = img[addr - CODE_BASE: addr - CODE_BASE + size]
    end = addr + size
    reloc = {}
    for o, info in fixups.items():
        if addr <= o < end:
            reloc[o] = ("data", info)
    # rel32 control flow whose target is outside the function
    a = addr
    while a < end:
        info = insns.get(a)
        if not info:
            a += 1; continue
        raw, opi = info
        if opi is not None and len(raw) >= opi + 4:
            rel = int.from_bytes(bytes(raw[opi:opi + 4]), "little", signed=True)
            tgt = (a + len(raw) + rel) & 0xffffffff
            if not (addr <= tgt < end):
                reloc[a + opi] = ("rel", tgt)
        a += len(raw)
    lines = ["    .text", "    .globl %s" % name, "%s:" % name]
    pend = []
    def flush():
        while pend:
            ch = pend[:16]; del pend[:16]
            lines.append("    .byte " + ",".join("0x%02x" % b for b in ch))
    exts = set()
    i = 0
    while i < size:
        o = addr + i
        if o in reloc:
            flush()
            kind, val = reloc[o]
            if kind == "data":
                objn, toff, add = val
                e = "__obj%d + 0x%x" % (objn, toff)
                if add:
                    e += " + 0x%x" % add
                lines.append("    .long %s" % e)
            else:
                sym = names.get(val, "fn_%x" % val)
                exts.add(sym)
                lines.append("    .long %s - . - 4" % sym)
            i += 4
        else:
            pend.append(data[i]); i += 1
    flush()
    exts.discard(name)
    return "\n".join(lines) + "\n", exts

def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "port", "gen", "asm")
    os.makedirs(outdir, exist_ok=True)
    fixups = all_fixups()
    img, insns = disasm_all()
    names = func_names()
    fns = json.load(open(os.path.join(ROOT, "manifest", "functions.json")))["functions"]
    fns = [f for f in fns if f.get("addr") and int(f.get("size", 0)) > 0]
    emitted, skipped, all_ext, defined = 0, [], set(), set()
    for f in fns:
        addr = int(f["addr"], 16); size = int(f["size"])
        name = f["name"]
        if addr < CODE_BASE:               # cut-off prefix -- bytes not in the file
            skipped.append(name); continue
        s, exts = emit_one(name, addr, size, img, insns, fixups, names)
        open(os.path.join(outdir, name + ".s"), "w").write(s)
        all_ext |= exts; defined.add(name); emitted += 1
    unresolved = sorted(e for e in all_ext if e not in defined)
    print("emitted %d .s into %s   (skipped %d prefix funcs)" % (emitted, outdir, len(skipped)))
    print("external symbols referenced but not defined here: %d" % len(unresolved))
    open(os.path.join(outdir, "_unresolved.txt"), "w").write("\n".join(unresolved) + "\n")
    open(os.path.join(outdir, "_prefix_skipped.txt"), "w").write("\n".join(skipped) + "\n")
    fn_addr = sorted(u for u in unresolved if u.startswith("fn_"))
    print("  fn_<addr> (callees not in manifest / prefix): %d" % len(fn_addr))
    print("  named externs (CLIB / boundary): %d" % (len(unresolved) - len(fn_addr)))
    print("  sample named:", [u for u in unresolved if not u.startswith("fn_")][:12])

if __name__ == "__main__":
    main()
