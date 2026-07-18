#!/usr/bin/env python3
"""gfxasm.py -- emit a READABLE, annotated assembly listing for the game's hand-written
graphics/sound routines (the db-transcribed lib/gfx and lib/sound functions, proven NOT
Watcom library code -- see docs/game-vs-library.md).

These functions are hand asm: every one saves EAX/ECX/EDX, which a C compiler never does,
so they can't be reconstructed from C and the build carries them as raw `#pragma aux "db"`
bytes in the .c file. That .c is the build input; this .asm is the human-readable companion,
disassembled from the original bytes with call targets and known globals resolved to names.

  python3 tools/gfxasm.py <subdir>          # e.g. lib/gfx  -> writes src/<subdir>/<name>.asm
"""
import json, os, glob, sys, re
import capstone

MAN = "manifest/functions.json"
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"

def load():
    man = json.load(open(MAN))
    base = int(man["image_base"], 16)
    seg = open(SEG, "rb").read()
    faddr = {int(f["addr"], 16): f["name"] for f in man["functions"]}
    g = json.load(open("manifest/globals.json")) if os.path.exists("manifest/globals.json") else {}
    gaddr = {}
    for name, v in g.items():
        try: gaddr[int(v["addr"], 16)] = name
        except Exception: pass
    return man, base, seg, faddr, gaddr

def annotate(name, f, base, seg, faddr, gaddr):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    off = int(f["addr"], 16) - base
    code = seg[off:off + f["size"]]
    lines = []
    memre = re.compile(r"\[(0x[0-9a-fA-F]+)\]")
    for ins in md.disasm(code, int(f["addr"], 16)):
        note = ""
        # resolve call/jmp targets to a function name
        if ins.mnemonic in ("call", "jmp", "je", "jne", "jl", "jge", "jg", "jle", "ja", "jb", "jae", "jbe"):
            m = re.fullmatch(r"0x[0-9a-fA-F]+", ins.op_str)
            if m:
                tgt = int(ins.op_str, 16)
                if tgt in faddr:
                    note = "-> %s" % faddr[tgt]
        # resolve absolute [global] operands
        for m in memre.finditer(ins.op_str):
            a = int(m.group(1), 16)
            if a in gaddr:
                note = (note + " " if note else "") + "%s=%s" % (m.group(1), gaddr[a])
        asm = "%-7s %s" % (ins.mnemonic, ins.op_str)
        raw = ins.bytes.hex()
        if note:
            lines.append("        %-40s ; %-14s %s" % (asm, raw, note))
        else:
            lines.append("        %-40s ; %s" % (asm, raw))
    return lines

def main():
    sub = sys.argv[1] if len(sys.argv) > 1 else "lib/gfx"
    man, base, seg, faddr, gaddr = load()
    byname = {f["name"]: f for f in man["functions"]}
    srcs = sorted(glob.glob("src/%s/*.c" % sub))
    n = skipped = 0
    for p in srcs:
        name = os.path.basename(p)[:-2]
        f = byname.get(name)
        if not f:
            continue
        # only the db-transcription (hand-asm) functions get a listing; the rest are
        # compiled C that already reads cleanly in the .c, so a disassembly adds nothing.
        ctext = open(p, encoding="utf-8", errors="replace").read()
        if not re.search(r'"db [0-9]|__db_FUN', ctext):
            skipped += 1
            continue
        out = p[:-2] + ".asm"
        # don't clobber a hand-curated listing (marked "(fully commented)" in its header).
        if os.path.exists(out) and "(fully commented)" in open(out, encoding="utf-8", errors="replace").read(2000):
            skipped += 1
            continue
        lines = annotate(name, f, base, seg, faddr, gaddr)
        hdr = [
            "; %s @ %s  (%d bytes) -- hand-written assembly, reconstructed listing." % (name, f["addr"], f["size"]),
            "; Original bytes disassembled from the game image; call targets and known globals",
            "; resolved to names. The build uses %s (db-transcription); this listing is the" % os.path.basename(p),
            "; readable companion. See docs/game-vs-library.md for why these are hand asm.",
            ";",
            "%s:" % name,
        ]
        open(out, "w", encoding="utf-8", newline="\n").write("\n".join(hdr + lines) + "\n")
        n += 1
    print("wrote %d .asm listings under src/%s/  (skipped %d compiled-C fns)" % (n, sub, skipped))

if __name__ == "__main__":
    main()
