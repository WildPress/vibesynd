#!/usr/bin/env python3
"""prefix_obj.py -- author ONE relink-safe OMF object for object1's recovered code PREFIX
(manifest [0xd748, 0x10000)), the region the shifted linear.bin dropped. It contains the game's
MAIN LOOP (FUN_0000d928) and ~a dozen more startup functions that buildgame.py currently stubs to
`ret`, which is why our GAME.EXE inits then exits with no render.

WHY one object (not per-function db files): the prefix interleaves code with data (padding, tables),
so carving only the function bodies would drop the in-between bytes. Emitting the WHOLE prefix as a
single _TEXT keeps every byte at its correct intra-prefix offset; PUBDEFs at each function entry let
the main body's `extern FUN_xxxx` calls resolve; and only references that LEAVE the prefix need fixups
(intra-prefix relative refs stay correct because the object relocates as a unit).

Byte source: build/obj1_full.bin (run tools/linearize.py first) -- the TRUE object1 image at
manifest base 0xd748, so prefix byte at manifest A = obj1_full.bin[A-0xd748].

Reloc model (recursive disassembly from seed entries stays on real code, skipping data regions):
  * rel32 call/jmp/jcc, target in [0xd748,0x10000)  -> INTRA-prefix: leave baked; call target is a
        new PUBDEF entry, jump target is followed.
  * rel32 call/jmp,     target >= 0x10000           -> EXTERNAL code: self-relative FIXUPP to the
        nearest main-body FUN_ (EXTDEF) so wlink recomputes it wherever the main body lands.
  * abs32 [disp32] / mov imm32 whose value hits an EXACT function start (prefix or main) -> abs
        FIXUPP to that FUN_ symbol. Non-exact code-range constants + all data disps stay baked
        (data lives at fixed DGROUP addresses, like the rest of the build).

  python3 tools/prefix_obj.py            # write build/prefix.obj (+ print entry/reloc summary)
  python3 tools/prefix_obj.py --disasm   # dump discovered entries + external relocs, no write
"""
import os, sys, json, struct
from capstone import (Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET,
                      x86_const)
X86_OP_IMM = x86_const.X86_OP_IMM
X86_OP_MEM = x86_const.X86_OP_MEM
ESP, EBP = x86_const.X86_REG_ESP, x86_const.X86_REG_EBP

FULL = "build/obj1_full.bin"
MAN  = "manifest/functions.json"
PREFIX_LO, PREFIX_HI = 0xd748, 0x10000          # the recovered prefix (manifest coords)
CODE_LO,   CODE_HI   = 0xd748, 0x4fdf4          # whole object1 code range
DATA_LO,   DATA_HI   = 0x100,  0x1c632

# Ghidra-carved prefix function entries + the stub-referenced prefix targets, used as recursion seeds.
SEEDS = [0xd758, 0xd928, 0xdaa8, 0xdc08, 0xe568, 0xe5a8, 0xf5e8, 0xf898,
         0xfa18, 0xfa88, 0xfb48, 0xfd38, 0xfee8, 0xffc8]

_md = Cs(CS_ARCH_X86, CS_MODE_32); _md.detail = True


def load_main_fns():
    man = json.load(open(MAN))
    return sorted((int(x["addr"], 16), x["size"], x["name"]) for x in man["functions"])


def containing_fn(fns, lin):
    lo, hi = 0, len(fns)
    while lo < hi:
        m = (lo + hi) // 2
        if fns[m][0] <= lin: lo = m + 1
        else: hi = m
    if lo:
        a, s, nm = fns[lo-1]
        return nm, lin - a, (a <= lin < a + s)
    return None, None, False


def traverse(P):
    """Recursive-descent disassembly from SEEDS over the prefix bytes P (base PREFIX_LO).
    Returns (entries:set of intra-prefix call targets incl seeds, relocs:list)."""
    entries = set(a for a in SEEDS if PREFIX_LO <= a < PREFIX_HI)
    seen = set()                                    # instruction start offsets already decoded
    relocs = []                                     # (off_in_prefix, size, self_rel, target)
    work = list(entries)
    while work:
        addr = work.pop()
        off = addr - PREFIX_LO
        if off in seen or not (0 <= off < len(P)):
            continue
        for insn in _md.disasm(P[off:], addr):
            io = insn.address - PREFIX_LO
            if io in seen:
                break
            seen.add(io)
            enc = insn.encoding
            branch = (CS_GRP_JUMP in insn.groups) or (CS_GRP_CALL in insn.groups)
            is_call = CS_GRP_CALL in insn.groups
            for op in insn.operands:
                if op.type == X86_OP_MEM and enc.disp_size == 4:
                    if op.mem.base in (ESP, EBP):
                        continue
                    v = op.mem.disp & 0xFFFFFFFF
                    if CODE_LO <= v <= CODE_HI:
                        relocs.append((io + enc.disp_offset, 4, False, v))
                elif op.type == X86_OP_IMM and enc.imm_size == 4:
                    v = op.imm & 0xFFFFFFFF
                    if branch:
                        if CODE_LO <= v <= CODE_HI:
                            if PREFIX_LO <= v < PREFIX_HI:
                                if is_call:
                                    entries.add(v)
                                if (v - PREFIX_LO) not in seen:
                                    work.append(v)           # follow intra-prefix flow
                            else:
                                relocs.append((io + enc.imm_offset, 4, True, v))  # -> main body
                    elif CODE_LO <= v <= CODE_HI:
                        relocs.append((io + enc.imm_offset, 4, False, v))
            # stop linear decode at a hard flow break; branch targets were queued above
            if CS_GRP_RET in insn.groups:
                break
            if (CS_GRP_JUMP in insn.groups) and not is_call and insn.mnemonic == "jmp":
                break
    return entries, relocs


# ---- OMF authoring (mirrors tools/unbake.py) ----
idx  = lambda v: bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])
pstr = lambda s: bytes([len(s)]) + s.encode("latin1")
def omf_rec(rt, body):
    ln = len(body) + 1
    return bytes([rt, ln & 0xff, ln >> 8]) + body + bytes([0])


def main():
    if not os.path.exists(FULL):
        sys.exit("missing %s -- run tools/linearize.py first" % FULL)
    P = bytearray(open(FULL, "rb").read()[:PREFIX_HI - PREFIX_LO])
    mainfns = load_main_fns()
    prefix_only = [(a, s, nm) for a, s, nm in mainfns if a >= PREFIX_HI]

    entries, relocs = traverse(P)

    # resolve each reloc to a symbol; intra-prefix targets -> our own PUBDEF, else -> main FUN_
    resolved = []
    for off, size, self_rel, tgt in relocs:
        if PREFIX_LO <= tgt < PREFIX_HI:
            nm = "FUN_%08x" % tgt                      # our own entry (must be a PUBDEF)
            if self_rel:                               # intra rel: baked-correct, skip fixup
                continue
            if tgt not in entries:                     # abs into prefix but not an entry -> keep baked
                continue
            addend = 0
        else:
            nm, addend, in_fn = containing_fn(prefix_only, tgt)
            if nm is None:
                continue
            if not self_rel and not (in_fn and addend == 0):
                continue                               # code-range constant, not a fn ptr -> baked
        orig = int.from_bytes(P[off:off+size], "little")
        resolved.append(dict(off=off, size=size, self_rel=self_rel, tgt=tgt,
                             sym=nm, addend=addend, orig=orig))

    ents = sorted(entries)
    if "--disasm" in sys.argv:
        print("prefix bytes: 0x%x  entries: %d  relocs(external): %d" % (len(P), len(ents), len(resolved)))
        print("entries:", " ".join("%x" % e for e in ents))
        for r in resolved[:60]:
            print("  +0x%-4x %s %-16s tgt=0x%x add=0x%x" %
                  (r["off"], "rel" if r["self_rel"] else "abs", r["sym"], r["tgt"], r["addend"]))
        return

    # zero the operands we fixup, build OMF
    for r in resolved:
        P[r["off"]:r["off"]+r["size"]] = b"\x00" * r["size"]
    exts = []
    for r in resolved:
        if r["tgt"] >= PREFIX_HI and r["sym"] not in exts:
            exts.append(r["sym"])
    # PUBDEF names: FUN_<addr> AND FUN_LE_<addr> alias (the missing-symbol set uses both forms)
    out = bytearray()
    out += omf_rec(0x80, pstr("PREFIX"))
    out += omf_rec(0x96, pstr("") + pstr("_TEXT") + pstr("CODE"))
    out += omf_rec(0x99, bytes([0x69]) + struct.pack("<I", len(P)) + idx(2) + idx(3) + idx(1))
    for s in exts:                                     # EXTDEF for main-body targets
        out += omf_rec(0x8C, pstr(s) + idx(0))
    # PUBDEF: every discovered entry, both name forms, at its prefix offset
    pub = bytearray()
    for e in ents:
        for nm in ("FUN_%08x" % e, "FUN_LE_%08x" % e):
            pub += pstr(nm) + struct.pack("<H", e - PREFIX_LO) + idx(0)
    out += omf_rec(0x90, idx(0) + idx(1) + bytes(pub))
    # LEDATA + FIXUPP in 1024-byte chunks
    off = 0
    while off < len(P):
        chunk = bytes(P[off:off+1024]); clen = len(chunk)
        out += omf_rec(0xA1, idx(1) + struct.pack("<I", off) + chunk)
        fb = bytearray()
        for r in resolved:
            if off <= r["off"] < off + clen:
                doff = r["off"] - off
                M = 0 if r["self_rel"] else 1
                locat = 0x8000 | (M << 14) | (9 << 10) | (doff & 0x3FF)
                fb += bytes([locat >> 8, locat & 0xFF])
                ei = exts.index(r["sym"]) + 1
                if r["addend"]:
                    fb += bytes([0x52]) + idx(ei) + struct.pack("<I", r["addend"])
                else:
                    fb += bytes([0x56]) + idx(ei)
        if fb:
            out += omf_rec(0x9D, bytes(fb))
        off += clen
    out += omf_rec(0x8B, bytes([0x00]))
    os.makedirs("build", exist_ok=True)
    open("build/prefix.obj", "wb").write(out)
    print("wrote build/prefix.obj: %d bytes, %d entries, %d external relocs (%d EXTDEFs)"
          % (len(P), len(ents), len(resolved), len(exts)))
    print("entries:", " ".join("%x" % e for e in ents))


if __name__ == "__main__":
    main()
