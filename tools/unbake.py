#!/usr/bin/env python3
"""unbake.py -- relocation recovery for db-transcribed (library/asm) functions, DISASSEMBLER-driven.

db-transcription baked each library function's operands as literal resolved bytes with zero
relocations, so those objects only work at the original addresses. This tool DISASSEMBLES each such
function (capstone, x86-32), finds every operand that is a cross-reference, and regenerates its .obj
with proper OMF FIXUPP records pointing at the existing FUN_/g_ symbols -- making them relink-safe at
any address, like the compiled-C game functions.

Reference kinds handled (validated model, see AGENTS.md):
  * rel32 call/jmp/jcc  -> code target  -> symbol FUN_<containing fn> (+addend)   [self-relative fixup]
  * memory [disp32] with a non-frame base (or none) -> data/code addr -> g_<v> / FUN_ [abs fixup]
  * mov reg,imm32 whose imm is in the code range -> code pointer -> FUN_          [abs fixup]

  python3 tools/unbake.py --disasm FUN_x ...   # show recovered relocation sites (debug)
  python3 tools/unbake.py --verify FUN_x ...   # regenerate in memory + prove bytes reproduce image
  python3 tools/unbake.py FUN_x ...            # write build/<name>.obj with recovered relocs
  python3 tools/unbake.py --all                # regenerate every db function; print data-global set
"""
import os, sys, json, struct
from capstone import (Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL,
                      x86_const)
X86_OP_IMM = x86_const.X86_OP_IMM
X86_OP_MEM = x86_const.X86_OP_MEM
ESP, EBP = x86_const.X86_REG_ESP, x86_const.X86_REG_EBP

SEG1 = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MAN  = "manifest/functions.json"
CODE_LO, CODE_HI = 0x10000, 0x4fdf4          # OBJ1 code range
DATA_LO, DATA_HI = 0x100, 0x1c632            # plausible data-global range (below code)

_md = Cs(CS_ARCH_X86, CS_MODE_32); _md.detail = True
_FNS = None


def load_fns():
    man = json.load(open(MAN))
    return sorted(((int(x["addr"], 16), x["size"], x["name"]) for x in man["functions"]))


def containing_fn(lin):
    """-> (name, addend, in_fn): the function whose range contains lin (in_fn True), else the
    nearest function at/below lin (in_fn False, a gap/un-carved target), else (None,None,False)."""
    lo, hi = 0, len(_FNS)
    while lo < hi:
        m = (lo + hi) // 2
        if _FNS[m][0] <= lin: lo = m + 1
        else: hi = m
    if lo:
        a, s, nm = _FNS[lo-1]
        return nm, lin - a, (a <= lin < a + s)
    return None, None, False


def find_relocs(fn_addr, code, fn_size):
    """Return [(off_in_fn, size, self_rel, target_lin, kind)] for EXTERNAL CODE references only.
    Data refs (baked disps/immediates) already work via dataimg at DGROUP:0, and internal jumps
    stay correct when the function relocates as a unit -- neither needs recovery. What breaks on
    relink is a reference to ANOTHER function's code, so those are all we symbolize:
      * rel32 call/jmp/jcc whose target is OUTSIDE this function   -> self-relative fixup -> FUN_
      * abs32 memory [disp32] into the code range (fn-ptr / jumptable base) -> abs fixup -> FUN_
      * mov reg,imm32 whose imm is in the code range (fn pointer)         -> abs fixup -> FUN_
    """
    sites = []
    fend = fn_addr + fn_size
    for insn in _md.disasm(code, fn_addr):
        enc = insn.encoding
        base_off = insn.address - fn_addr
        branch = CS_GRP_JUMP in insn.groups or CS_GRP_CALL in insn.groups
        for op in insn.operands:
            if op.type == X86_OP_MEM and enc.disp_size == 4:
                if op.mem.base in (ESP, EBP):
                    continue
                v = op.mem.disp & 0xFFFFFFFF
                if CODE_LO <= v <= CODE_HI:                     # absolute pointer into code
                    sites.append((base_off + enc.disp_offset, 4, False, v, "code"))
            elif op.type == X86_OP_IMM and enc.imm_size == 4:
                v = op.imm & 0xFFFFFFFF
                if not (CODE_LO <= v <= CODE_HI):
                    continue
                if branch:
                    if fn_addr <= v < fend:                     # internal jump: leave baked
                        continue
                    sites.append((base_off + enc.imm_offset, 4, True, v, "code"))
                else:
                    sites.append((base_off + enc.imm_offset, 4, False, v, "code"))
    return sites


def symbol_for(target, self_rel):
    """A branch always references code (accept gap targets via nearest fn). A non-branch code-range
    operand is only a real pointer if it hits an EXACT function START (addend 0); otherwise it's a
    numeric constant that must stay baked -> return (None, ...) so the caller drops it."""
    nm, addend, in_fn = containing_fn(target)
    if self_rel:
        return nm, addend                              # external branch: keep even into a gap
    if nm is not None and in_fn and addend == 0:
        return nm, 0                                   # exact function-start pointer
    return None, 0                                     # constant -> leave baked


# ---- OMF authoring ----
idx  = lambda v: bytes([v]) if v < 0x80 else bytes([0x80 | (v >> 8), v & 0xff])
pstr = lambda s: bytes([len(s)]) + s.encode("latin1")
def omf_rec(rt, body):
    ln = len(body) + 1
    return bytes([rt, ln & 0xff, ln >> 8]) + body + bytes([0])


def build(name, fn_addr, fn_size, mode):
    if fn_addr < CODE_LO and os.path.exists("build/obj1_full.bin"):   # prefix fn: not in linear.bin
        body = bytearray(open("build/obj1_full.bin", "rb").read()[fn_addr - 0xd748:fn_addr - 0xd748 + fn_size])
    else:
        seg = open(SEG1, "rb").read()
        body = bytearray(seg[fn_addr - CODE_LO: fn_addr - CODE_LO + fn_size])
    relocs = find_relocs(fn_addr, bytes(body), fn_size)
    resolved = []
    for off, size, self_rel, tgt, kind in relocs:
        sym, addend = symbol_for(tgt, self_rel)
        if sym is None:                                # numeric constant in code range -> keep baked
            continue
        orig = int.from_bytes(body[off:off+size], "little")
        resolved.append(dict(off=off, size=size, self_rel=self_rel, tgt=tgt,
                             kind=kind, sym=sym, addend=addend, orig=orig))

    if mode == "disasm":
        print("%s @0x%x (%dB): %d relocs" % (name, fn_addr, fn_size, len(resolved)))
        for r in resolved:
            print("  +0x%-4x %s %-14s tgt=0x%x addend=0x%x orig=0x%x"
                  % (r["off"], "rel" if r["self_rel"] else "abs", r["sym"], r["tgt"], r["addend"], r["orig"]))
        return resolved

    if mode == "verify":
        ok = True
        for r in resolved:
            if r["sym"] is None:
                print("  +0x%x UNRESOLVED tgt=0x%x" % (r["off"], r["tgt"])); ok = False; continue
            loc = fn_addr + r["off"]
            base = r["tgt"]                            # symbol resolves to target linear (orig layout)
            val = (base - (loc + r["size"])) if r["self_rel"] else base
            val &= 0xFFFFFFFF
            if val != r["orig"]:
                ok = False
                print("  +0x%x %s recomputed=0x%x orig=0x%x MISMATCH" % (r["off"], r["sym"], val, r["orig"]))
        print("VERIFY %s: %s (%d relocs)" % (name, "PASS" if ok else "FAIL", len(resolved)))
        return ok

    # write mode: zero operands, emit OMF with fixups
    for r in resolved:
        body[r["off"]:r["off"]+r["size"]] = b"\x00" * r["size"]
    syms = []
    for r in resolved:
        if r["sym"] and r["sym"] not in syms:
            syms.append(r["sym"])
    out = bytearray()
    out += omf_rec(0x80, pstr(name))
    # LNAMES: 1="" 2="_TEXT" 3="CODE"; SEGDEF segname=_TEXT(2) class=CODE(3) ovl=""(1)
    out += omf_rec(0x96, pstr("") + pstr("_TEXT") + pstr("CODE"))
    out += omf_rec(0x99, bytes([0x69]) + struct.pack("<I", fn_size) + idx(2) + idx(3) + idx(1))
    for s in syms:
        out += omf_rec(0x8C, pstr(s) + idx(0))
    out += omf_rec(0x90, idx(0) + idx(1) + pstr(name) + struct.pack("<H", 0) + idx(0))
    off = 0
    while off < len(body):
        chunk = bytes(body[off:off+1024]); clen = len(chunk)
        out += omf_rec(0xA1, idx(1) + struct.pack("<I", off) + chunk)
        fb = bytearray()
        for r in resolved:
            if r["sym"] and off <= r["off"] < off + clen:
                doff = r["off"] - off
                M = 0 if r["self_rel"] else 1
                locat = 0x8000 | (M << 14) | (9 << 10) | (doff & 0x3FF)
                fb += bytes([locat >> 8, locat & 0xFF])
                ei = syms.index(r["sym"]) + 1
                if r["addend"]:
                    fb += bytes([0x52]) + idx(ei) + struct.pack("<I", r["addend"])
                else:
                    fb += bytes([0x56]) + idx(ei)
        if fb:
            out += omf_rec(0x9D, bytes(fb))
        off += clen
    out += omf_rec(0x8B, bytes([0x00]))
    open("build/%s.obj" % name, "wb").write(out)
    return resolved


def is_db(name):
    import glob as _g
    hits = _g.glob("src/**/%s.c" % name, recursive=True)
    return bool(hits) and "__db_" in open(hits[0], encoding="latin1", errors="replace").read()


def main():
    global _FNS
    _FNS = load_fns()
    by = {nm: (a, s) for a, s, nm in _FNS}
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    mode = ("disasm" if "--disasm" in sys.argv else
            "verify" if "--verify" in sys.argv else "write")
    if "--all" in sys.argv:
        args = [nm for a, s, nm in _FNS if is_db(nm)]
    data_globals, npass, nfail = set(), 0, 0
    for name in args:
        a, s = by[name]
        r = build(name, a, s, mode)
        if mode == "verify":
            if r: npass += 1
            else: nfail += 1
        elif mode == "write":
            for rr in r:
                if rr["kind"] == "data" and rr["sym"]:
                    data_globals.add(rr["tgt"])
    if mode == "verify":
        print("\n%d PASS, %d FAIL" % (npass, nfail))
    elif mode == "write" and "--all" in sys.argv:
        print("wrote %d db objects; %d distinct data globals referenced" % (len(args), len(data_globals)))
        json.dump(sorted(data_globals), open("build/db_data_globals.json", "w"))


if __name__ == "__main__":
    main()
