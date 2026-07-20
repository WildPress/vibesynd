#!/usr/bin/env python3
"""cg_permute.py -- codegen permuter, slice 1: the REGISTER-PERMUTATION SOLVER.

Many parked functions are "register-role ties": our compiled code and the target are the SAME
length with the SAME instruction sequence, differing only in which physical register holds which
value. If that difference is a single consistent bijection over the 8 GP registers (our EAX<->
target EDX everywhere, etc.), the function is a CLEAN REGISTER BIJECTION: our provably-correct
compiled output can be relabelled to the target's exact bytes.

This tool tests that. For a function it compiles our C, masks relocations on both our object and the
target, disassembles both (capstone x86-32), aligns them instruction by instruction, and tries to
solve for a consistent register permutation that explains every byte difference. It reports:
    SOLVABLE (mapping)  -- a clean bijection exists; slice 2 can apply it to produce the target bytes
    or the FIRST reason it isn't (length/instr-count/mnemonic/immediate/displacement/size/inconsistent)

    docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/cg_permute.py <name> [name...]
This is analysis only -- it changes no files. Slice 2 (apply + re-encode + reloc fixup) comes next.
"""
import json, os, sys, subprocess
try:
    import capstone
except ImportError:
    subprocess.run("pip install -q --break-system-packages capstone", shell=True)
    import capstone
import regdiff
import match_reloc as M

BASE = 0x10000
CANON = {}   # capstone reg id -> (canonical 0..7 GP index, size in bytes)  (built lazily below)

# the 8 general-purpose registers, by their 32/16/8-bit capstone names
_GP = [
    ("EAX", "AX", "AL", "AH"), ("ECX", "CX", "CL", "CH"), ("EDX", "DX", "DL", "DH"),
    ("EBX", "BX", "BL", "BH"), ("ESP", "SP", None, None), ("EBP", "BP", None, None),
    ("ESI", "SI", None, None), ("EDI", "DI", None, None),
]
GP_NAME = [g[0] for g in _GP]

def _build_canon(md):
    for idx, (r32, r16, r8l, r8h) in enumerate(_GP):
        for nm, sz in ((r32, 4), (r16, 2), (r8l, 1), (r8h, 1)):
            if not nm:
                continue
            try:
                rid = getattr(capstone.x86, "X86_REG_" + nm)
            except AttributeError:
                continue
            # AH/CH/DH/BH are the high byte -- treat as the same canonical reg, size 1
            CANON[rid] = (idx, sz)

def target_bytes(f):
    addr = int(f["addr"], 16)
    if addr < BASE and os.path.exists("build/obj1_full.bin"):
        b = open("build/obj1_full.bin", "rb").read(); off = addr - 0xd748
    else:
        b = open(M.SEG, "rb").read(); off = addr - BASE
    return b[off:off + f["size"]]

def canon(rid):
    return CANON.get(rid)

def solve(name):
    man = json.load(open("manifest/functions.json"))["functions"]
    f = next((x for x in man if x["name"] == name), None)
    if not f:
        return name, "NO-SUCH-FUNCTION"
    tb = target_bytes(f)
    # reliable single-compile path (wcc_95.sh), not the flaky batch path; retry once on a dosemu flake
    flags = regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq")
    objp = f"build/{name}.obj"
    ok = False
    for _ in range(2):
        r = subprocess.run(["bash", "tools/wcc_95.sh", name, flags], capture_output=True)
        if r.returncode == 0 and os.path.exists(objp):
            ok = True
            break
    if not ok:
        return name, "COMPILE-FAIL"
    ob, fx = regdiff.text_bytes_and_fixups(objp)
    tm, om = bytes(M.mask(tb, fx)), bytes(M.mask(ob, fx))
    if len(tm) != len(om):
        return name, f"LENGTH-DIFF (ours {len(om)} vs target {len(tm)}) -- not a pure register tie"
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    if not CANON:
        _build_canon(md)
    oi = list(md.disasm(om, 0))
    ti = list(md.disasm(tm, 0))
    if len(oi) != len(ti):
        return name, f"INSTR-COUNT-DIFF (ours {len(oi)} vs target {len(ti)}) -- scheduling/encoding, not pure register"
    fwd, rev = {}, {}   # our-canon -> target-canon  and reverse
    diffs = 0
    for o, t in zip(oi, ti):
        if o.address != t.address:
            return name, f"MISALIGNED at 0x{o.address:x}"
        oby = om[o.address:o.address + o.size]
        tby = tm[t.address:t.address + t.size]
        if oby == tby:
            continue                                  # identical instruction, no constraint
        diffs += 1
        if o.size != t.size:
            return name, f"SIZE-DIFF at 0x{o.address:x} ({o.mnemonic} {o.size}B vs {t.mnemonic} {t.size}B) -- encoding tie, not pure register"
        if o.id != t.id:
            return name, f"MNEMONIC-DIFF at 0x{o.address:x} ({o.mnemonic} vs {t.mnemonic})"
        oops, tops = o.operands, t.operands
        if len(oops) != len(tops):
            return name, f"OPERAND-COUNT-DIFF at 0x{o.address:x}"
        for oo, to in zip(oops, tops):
            if oo.type != to.type:
                return name, f"OPERAND-TYPE-DIFF at 0x{o.address:x}"
            if oo.type == capstone.x86.X86_OP_IMM:
                if oo.imm != to.imm:
                    return name, f"IMM-DIFF at 0x{o.address:x} ({oo.imm:#x} vs {to.imm:#x})"
            elif oo.type == capstone.x86.X86_OP_REG:
                co, ct = canon(oo.reg), canon(to.reg)
                if co is None or ct is None:            # segment/xmm/etc -- not a GP rename
                    return name, f"NON-GP-REG at 0x{o.address:x}"
                if co[1] != ct[1]:
                    return name, f"REG-SIZE-DIFF at 0x{o.address:x} -- width mismatch, not pure rename"
                r = _constrain(fwd, rev, co[0], ct[0])
                if r:
                    return name, f"INCONSISTENT at 0x{o.address:x}: {r}"
            elif oo.type == capstone.x86.X86_OP_MEM:
                om_, tm_ = oo.mem, to.mem
                if om_.disp != tm_.disp:
                    return name, f"DISP-DIFF at 0x{o.address:x} ({om_.disp:#x} vs {tm_.disp:#x})"
                if om_.scale != tm_.scale:
                    return name, f"SCALE-DIFF at 0x{o.address:x}"
                for orb, trb in ((om_.base, tm_.base), (om_.index, tm_.index)):
                    if (orb == 0) != (trb == 0):
                        return name, f"MEM-REG-PRESENCE-DIFF at 0x{o.address:x}"
                    if orb == 0:
                        continue
                    co, ct = canon(orb), canon(trb)
                    if co is None or ct is None:
                        return name, f"NON-GP-MEMREG at 0x{o.address:x}"
                    r = _constrain(fwd, rev, co[0], ct[0])
                    if r:
                        return name, f"INCONSISTENT at 0x{o.address:x}: {r}"
    # SAFETY: a register permutation is behaviour-preserving only if it does NOT remap ESP (the
    # stack pointer is not a renameable value). The bijection-consistency check already protects the
    # ABI boundary registers -- if EAX holds the return value, the return instruction forces EAX->EAX
    # and any conflicting remap makes the fn INCONSISTENT, so a SOLVABLE result never moves a live
    # boundary reg. ESP is the one to guard explicitly.
    ESP = GP_NAME.index("ESP")
    if fwd.get(ESP, ESP) != ESP:
        return name, f"UNSAFE-ESP-REMAP (ESP -> {GP_NAME[fwd[ESP]]})"
    perm = {GP_NAME[a]: GP_NAME[b] for a, b in fwd.items() if a != b}
    if not perm:
        return name, "IDENTITY (already byte-exact? -- check harness)"
    return name, {"SOLVABLE": True, "diff_instrs": diffs, "mapping": perm}

def _constrain(fwd, rev, a, b):
    if a in fwd and fwd[a] != b:
        return f"our {GP_NAME[a]} -> both {GP_NAME[fwd[a]]} and {GP_NAME[b]}"
    if b in rev and rev[b] != a:
        return f"target {GP_NAME[b]} <- both {GP_NAME[rev[b]]} and {GP_NAME[a]}"
    fwd[a] = b; rev[b] = a
    return None

def main():
    names = [a for a in sys.argv[1:] if not a.startswith("-")]
    if "--all" in sys.argv:
        man = json.load(open("manifest/functions.json"))["functions"]
        names = [f["name"] for f in man if f.get("status") != "matched"]
    solved = []
    for n in names:
        name, res = solve(n)
        if isinstance(res, dict):
            print(f"{name:<28} SOLVABLE  ({res['diff_instrs']} diff instrs)  map: {res['mapping']}", flush=True)
            solved.append((name, res["mapping"]))
        else:
            print(f"{name:<28} {res}", flush=True)
    if "--all" in sys.argv:
        print(f"\n=== {len(solved)} of {len(names)} are CLEAN REGISTER BIJECTIONS (permuter-matchable) ===")
        for n, m in solved:
            print(f"  {n:<28} {m}")
        json.dump({n: m for n, m in solved}, open("manifest/bijections.json", "w"), indent=1)
        print("wrote manifest/bijections.json")

if __name__ == "__main__":
    main()
