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
OBJ1 = os.path.join(ROOT, "build", "obj1_full.bin")   # true object1 at base 0xd748 (incl. prefix)
CODE_BASE = 0xd748
CODE_HI = 0x4fdf4     # end of object1 code (lefix: obj1 base 0x10000 + vsize 0x3fdf4)
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

def emit_one(name, addr, size, img, insns, fixups, names, labels=None):
    """labels: {abs_offset: symbol} -> emit `.globl sym; sym:` before that byte (for internal
    entry points that other functions call into)."""
    data = img[addr - CODE_BASE: addr - CODE_BASE + size]
    end = addr + size
    labels = labels or {}
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
            # only inter-function calls to REAL code; a target outside the code range is
            # data mis-decoded in a gap blob -> leave the bytes verbatim
            if not (addr <= tgt < end) and CODE_BASE <= tgt < CODE_HI:
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
        if o in labels:
            flush()
            lines.append("    .globl %s" % labels[o])
            lines.append("%s:" % labels[o])
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

CODE_HI = 0x4fdf4    # end of object1 code (lefix: obj1 base 0x10000 + vsize 0x3fdf4)

def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "port", "gen", "asm")
    os.makedirs(outdir, exist_ok=True)
    fixups = all_fixups()
    img, insns = disasm_all()
    names = func_names()
    fns = json.load(open(os.path.join(ROOT, "manifest", "functions.json")))["functions"]
    fns = [(int(f["addr"], 16), int(f["size"]), f["name"])
           for f in fns if f.get("addr") and int(f.get("size", 0)) > 0]
    fns.sort()
    spans = [(a, a + sz, n) for a, sz, n in fns]

    def containing(a):
        for s, e, n in spans:
            if s <= a < e:
                return (s, e, n)
        return None

    # units = (addr, size, name); grows as gap blobs are discovered. Track referenced targets.
    units = {name: (a, sz) for a, sz, name in fns}
    all_ext, defined = set(), set()

    def emit(name, a, sz, labels=None):
        s, exts = emit_one(name, a, sz, img, insns, fixups, names, labels)
        open(os.path.join(outdir, name + ".s"), "w").write(s)
        return exts

    # pass 1: emit carved functions, discover gap blobs iteratively
    for name, (a, sz) in list(units.items()):
        all_ext |= emit(name, a, sz); defined.add(name)
    starts = sorted(a for a, _, _ in fns)
    def next_start(a):
        return next((s for s in starts if s > a), CODE_HI)
    blobs = {}
    changed = True
    while changed:
        changed = False
        for e in sorted(e for e in all_ext if e.startswith("fn_") and e not in defined):
            a = int(e[3:], 16)
            if containing(a) or not (CODE_BASE <= a < CODE_HI):
                continue                            # internal entry (handled in pass 2) or non-code
            sz = next_start(a) - a
            all_ext |= emit(e, a, sz); defined.add(e); blobs[e] = sz
            spans.append((a, a + sz, e)); units[e] = (a, sz)
            starts = sorted(starts + [a]); changed = True

    # pass 2: internal entry points (a call target inside a unit, not at its start) become
    # real labels inside that unit -> re-emit the hosts that contain them.
    host_labels = {}
    n_internal = 0
    for e in sorted(e for e in all_ext if e.startswith("fn_") and e not in defined):
        a = int(e[3:], 16)
        c = containing(a)
        if c:
            host_labels.setdefault(c[2], {})[a] = e
            n_internal += 1
    for host, labels in host_labels.items():
        a, sz = units[host]
        emit(host, a, sz, labels)               # re-emit with the internal labels
        defined |= set(labels.values())

    unresolved = sorted(e for e in all_ext if e not in defined)
    open(os.path.join(outdir, "_unresolved.txt"), "w").write("\n".join(unresolved) + "\n")
    print("emitted %d carved + %d gap-blob functions; %d internal-entry labels in %d hosts"
          % (len(fns), len(blobs), n_internal, len(host_labels)))
    print("unresolved after closure: %d  %s" % (len(unresolved), unresolved[:16]))

if __name__ == "__main__":
    main()
