#!/usr/bin/env python3
"""asm_symbolize.py -- turn a byte-transcribed asm function into a RELOCATABLE .s.

The db-transcriptions carry offsets baked for the original 0x10000/DGROUP layout, so
they only run when everything loads there (the DOS build). To link them into a NATIVE
binary (branch `port`) alongside C, two classes of reference must become symbolic:

  * absolute data refs (abs32 into DGROUP) -- every one is a type-7 LE fixup, so we take
    the exact source offsets from tools/lefix.py and rewrite the 4-byte slot to
    `.long __dgroup + <flat addr>` (port_data.py places every global at __dgroup+addr).
  * inter-function calls/jumps (rel32 to a DIFFERENT function) -- found by disassembly;
    rewritten to `.long <callee> - . - 4` so the linker patches the correct native target
    (which may be the C reimplementation of that function).

Everything else -- including all INTRA-function jumps -- is emitted verbatim as .byte, so
the function's internal byte layout is preserved exactly and self-relative offsets stay
valid. That makes the emission trivially correct, verified by a byte-identity round trip:
assemble, link at the original address with __dgroup=0 and callees at their original
addresses, and the resulting .text must equal the original function bytes.

Usage:
  python tools/asm_symbolize.py <name> <hexaddr> <size>   # emit .s to stdout
  python tools/asm_symbolize.py --verify <name> <hexaddr> <size>   # round-trip byte check
"""
import sys, os, re, json, subprocess

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ1 = os.path.join(ROOT, "inputs", "SYNDICAT_MAIN_OBJECT1.linear.bin")
CODE_BASE = 0x10000            # OBJECT1 maps here (manifest space)

IS_WIN = sys.platform == "win32"    # on Windows binutils live in WSL; on Linux they're native

def sh(cmd):
    """Run a shell command in the binutils environment (WSL from Windows, native on Linux)."""
    argv = ["wsl", "bash", "-c", cmd] if IS_WIN else ["bash", "-c", cmd]
    return subprocess.run(argv, capture_output=True, text=True)

def _wslpath(p):
    return subprocess.run(["wsl", "wslpath", "-a", p.replace("\\", "/")],
                          capture_output=True, text=True).stdout.strip()

_OBJ1_WSL = None
def obj1_wsl():
    """Path to OBJECT1 as the binutils env sees it."""
    global _OBJ1_WSL
    if _OBJ1_WSL is None:
        _OBJ1_WSL = _wslpath(OBJ1) if IS_WIN else OBJ1
    return _OBJ1_WSL

def root_in_env():
    """Repo root as the binutils env sees it."""
    return obj1_wsl().rsplit("/inputs/", 1)[0]

def code_image():
    return open(OBJ1, "rb").read()

def func_bytes(addr, size):
    off = addr - CODE_BASE
    return code_image()[off:off + size]

def func_names():
    """addr(int) -> name, from the manifest."""
    m = {}
    for f in json.load(open(os.path.join(ROOT, "manifest", "functions.json")))["functions"]:
        try:
            m[int(f["addr"], 16)] = f["name"]
        except (KeyError, ValueError):
            pass
    return m

def data_fixups(addr, size):
    """Set of source offsets (abs) of type-7 data fixups within [addr, addr+size)."""
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "lefix.py"),
                          "src", "%x" % addr, "0x%x" % size],
                         capture_output=True, text=True).stdout
    offs = set()
    for line in out.splitlines():
        m = re.match(r"src=0x([0-9a-fA-F]+)\s+stype=7", line)
        if m:
            offs.add(int(m.group(1), 16))
    return offs

# rel32 control-flow opcodes: E8 call, E9 jmp, 0F 80..8F jcc
def rel32_sites(addr, size):
    """Return list of (operand_file_offset, target_abs) for rel32 control flow whose
    target lies OUTSIDE [addr, addr+size)."""
    end = addr + size
    cmd = ("objdump -D -b binary -m i386 -M intel --adjust-vma=0x%x "
           "--start-address=0x%x --stop-address=0x%x '%s'"
           % (CODE_BASE, addr, end, obj1_wsl()))
    txt = sh(cmd).stdout
    sites = []
    for line in txt.splitlines():
        m = re.match(r"\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2} )+)\s+(\S+)\s*(.*)", line)
        if not m:
            continue
        ia = int(m.group(1), 16)
        raw = [int(b, 16) for b in m.group(2).split()]
        mnem = m.group(3)
        # identify rel32 by opcode
        opi = 0
        is_rel32 = False
        if raw[0] == 0xE8 or raw[0] == 0xE9:
            is_rel32 = True; opi = 1
        elif raw[0] == 0x0F and 0x80 <= raw[1] <= 0x8F:
            is_rel32 = True; opi = 2
        if not is_rel32 or len(raw) < opi + 4:
            continue
        rel = int.from_bytes(bytes(raw[opi:opi + 4]), "little", signed=True)
        target = (ia + len(raw) + rel) & 0xffffffff
        if not (addr <= target < end):
            operand_off = ia + opi           # abs file/manifest offset of the 4-byte rel
            sites.append((operand_off, target))
    return sites

def symbolize(name, addr, size):
    data = func_bytes(addr, size)
    names = func_names()
    dfix = data_fixups(addr, size)
    rels = {off: tgt for off, tgt in rel32_sites(addr, size)}

    # reloc map: offset(abs) -> ('data', flat_addr) or ('rel', target_abs)
    reloc = {}
    for o in dfix:
        v = int.from_bytes(data[o - addr:o - addr + 4], "little")
        reloc[o] = ("data", v)
    for o, tgt in rels.items():
        reloc[o] = ("rel", tgt)

    lines = []
    lines.append("    .text")
    lines.append("    .globl %s" % name)
    lines.append("%s:" % name)
    i = 0
    pend = []                                # pending raw bytes
    def flush():
        while pend:
            chunk = pend[:16]; del pend[:16]
            lines.append("    .byte " + ",".join("0x%02x" % b for b in chunk))
    while i < size:
        o = addr + i
        if o in reloc:
            flush()
            kind, val = reloc[o]
            if kind == "data":
                lines.append("    .long __dgroup + 0x%x" % val)
            else:
                sym = names.get(val, "fn_%x" % val)
                lines.append("    .long %s - . - 4" % sym)
            i += 4
        else:
            pend.append(data[i]); i += 1
    flush()
    # externs: any callee symbol not this function
    exts = set()
    for o, (kind, val) in reloc.items():
        if kind == "rel":
            exts.add(names.get(val, "fn_%x" % val))
    exts.discard(name)
    hdr = ["/* generated by tools/asm_symbolize.py -- relocatable asm for %s @0x%x (%d bytes) */"
           % (name, addr, size)]
    for e in sorted(exts):
        hdr.append("    .extern %s" % e)
    hdr.append("")
    return "\n".join(hdr + lines) + "\n", exts

def verify(name, addr, size):
    """Round trip: symbolize -> assemble -> link at original addr with __dgroup=0 and all
    callees at their original addresses -> compare .text to the original bytes."""
    s, exts = symbolize(name, addr, size)
    names = func_names()
    addr_of = {v: k for k, v in names.items()}
    d = os.path.join(ROOT, "build", "asmsym")
    os.makedirs(d, exist_ok=True)
    sp = os.path.join(d, "f.s"); bp = os.path.join(d, "f.bin")
    open(sp, "w").write(s)
    dw = root_in_env()                            # repo root as binutils sees it
    spu = dw + "/build/asmsym/f.s"; opu = dw + "/build/asmsym/f.o"
    epu = dw + "/build/asmsym/f.elf"; bpu = dw + "/build/asmsym/f.bin"
    wsl = sh
    r = wsl("as --32 -o '%s' '%s'" % (opu, spu))
    if r.returncode:
        return False, "assemble failed:\n" + r.stderr
    defs = ["--defsym __dgroup=0"]
    for e in exts:
        if e.startswith("fn_"):
            defs.append("--defsym %s=0x%x" % (e, int(e[3:], 16)))
        elif e in addr_of:
            defs.append("--defsym %s=0x%x" % (e, addr_of[e]))
    r = wsl("ld -m elf_i386 %s -Ttext=0x%x -e %s -o '%s' '%s'"
            % (" ".join(defs), addr, name, epu, opu))
    if r.returncode:
        return False, "link failed:\n" + r.stderr
    wsl("objcopy -O binary --only-section=.text '%s' '%s'" % (epu, bpu))
    got = open(bp, "rb").read()
    want = func_bytes(addr, size)
    if got == want:
        return True, "byte-identical (%d bytes)" % size
    # diff first mismatch
    n = min(len(got), len(want))
    for j in range(n):
        if got[j] != want[j]:
            return False, "mismatch at +0x%x: got %02x want %02x (len got=%d want=%d)" % (
                j, got[j], want[j], len(got), len(want))
    return False, "length differs: got=%d want=%d" % (len(got), len(want))

if __name__ == "__main__":
    args = sys.argv[1:]
    if args and args[0] == "--verify":
        name, addr, size = args[1], int(args[2], 16), int(args[3])
        ok, msg = verify(name, addr, size)
        print(("OK   " if ok else "FAIL ") + name + ": " + msg)
        sys.exit(0 if ok else 1)
    else:
        name, addr, size = args[0], int(args[1], 16), int(args[2])
        s, _ = symbolize(name, addr, size)
        sys.stdout.write(s)
