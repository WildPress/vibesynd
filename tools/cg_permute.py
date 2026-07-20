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
try:
    import keystone
except ImportError:
    subprocess.run("pip install -q --break-system-packages keystone-engine", shell=True)
    try:
        import keystone
    except ImportError:
        keystone = None   # only the --apply path needs it
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

ENC_SWAPS = False   # --enc: also allow value-equivalent same-length encoding swaps (lea<->add/sub)

def _enc_equiv(o, t):
    """If our instruction o and target t are a KNOWN value-equivalent, same-length pair (an in-place
    `lea r,[r+d]` vs `add r,d`, or `lea r,[r-d]` vs `sub r,d`), return the (our_canon, target_canon)
    register constraint they impose, else None. Both being valid compilations of equivalent C, an
    in-place lea and the matching add compute the same value, and any flag difference is dead (else a
    correct compiler would not have chosen the flag-free lea), so the swap is behaviour-preserving."""
    X = capstone.x86
    LEA, ADD, SUB = X.X86_INS_LEA, X.X86_INS_ADD, X.X86_INS_SUB
    def lea_v(i):                          # in-place lea r,[r + disp] (single base, no index) -> (canon, disp)
        ops = i.operands
        if len(ops) == 2 and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_MEM:
            m = ops[1].mem
            d, b = canon(ops[0].reg), (canon(m.base) if m.base else None)
            if m.index == 0 and d and b and d[0] == b[0]:
                return d[0], m.disp
        return None
    def as_v(i):                           # add/sub r, imm -> (canon, effective +disp)
        ops = i.operands
        if len(ops) == 2 and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_IMM:
            c = canon(ops[0].reg)
            if c:
                return c[0], (-ops[1].imm if i.id == SUB else ops[1].imm)
        return None
    if o.id == LEA and t.id in (ADD, SUB):
        lv, av = lea_v(o), as_v(t)
        if lv and av and lv[1] == av[1]:
            return (lv[0], av[0])
    if o.id in (ADD, SUB) and t.id == LEA:
        av, lv = as_v(o), lea_v(t)
        if lv and av and lv[1] == av[1]:
            return (av[0], lv[0])
    return None

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
    diffs = swaps = 0
    for o, t in zip(oi, ti):
        if o.address != t.address:
            return name, f"MISALIGNED at 0x{o.address:x}"
        oby = om[o.address:o.address + o.size]
        tby = tm[t.address:t.address + t.size]
        if oby == tby:
            # IDENTICAL instruction: every GP register it names is FIXED (maps to itself). Recording
            # these identity constraints is essential -- without them the solver would accept a
            # remap of a register that also appears unchanged elsewhere (e.g. add eax,0 stays eax but
            # a remap says eax->edx), a false bijection the applier can't realise.
            for oo in o.operands:
                for rid in ((oo.reg,) if oo.type == capstone.x86.X86_OP_REG else
                            (oo.mem.base, oo.mem.index) if oo.type == capstone.x86.X86_OP_MEM else ()):
                    cc = canon(rid) if rid else None
                    if cc:
                        rr = _constrain(fwd, rev, cc[0], cc[0])
                        if rr:
                            return name, f"INCONSISTENT (fixed reg in identical insn) at 0x{o.address:x}: {rr}"
            continue
        diffs += 1
        if ENC_SWAPS and o.size == t.size and o.id != t.id:
            eq = _enc_equiv(o, t)
            if eq is not None:
                r = _constrain(fwd, rev, eq[0], eq[1])
                if r:
                    return name, f"INCONSISTENT (enc-swap) at 0x{o.address:x}: {r}"
                swaps += 1
                continue
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
    return name, {"SOLVABLE": True, "diff_instrs": diffs, "enc_swaps": swaps, "mapping": perm}

def _constrain(fwd, rev, a, b):
    if a in fwd and fwd[a] != b:
        return f"our {GP_NAME[a]} -> both {GP_NAME[fwd[a]]} and {GP_NAME[b]}"
    if b in rev and rev[b] != a:
        return f"target {GP_NAME[b]} <- both {GP_NAME[rev[b]]} and {GP_NAME[a]}"
    fwd[a] = b; rev[b] = a
    return None

REGCODE = {"EAX": 0, "ECX": 1, "EDX": 2, "EBX": 3, "ESP": 4, "EBP": 5, "ESI": 6, "EDI": 7}
SUBREGS = {"EAX": ("eax", "ax", "al", "ah"), "ECX": ("ecx", "cx", "cl", "ch"),
           "EDX": ("edx", "dx", "dl", "dh"), "EBX": ("ebx", "bx", "bl", "bh"),
           "ESP": ("esp", "sp", None, None), "EBP": ("ebp", "bp", None, None),
           "ESI": ("esi", "si", None, None), "EDI": ("edi", "di", None, None)}

def _token_map(mapping):
    full = {c: c for c in REGCODE}; full.update(mapping)
    tok = {}
    for s, d in full.items():
        for i, sr in enumerate(SUBREGS[s]):
            if sr and SUBREGS[d][i]:
                tok[sr] = SUBREGS[d][i]
    return tok

def apply_and_verify(name):
    """Slice 2: derive the target's exact bytes from OUR compiled output by re-assembling only the
    register-differing instructions with the permutation applied (everything else keeps our bytes,
    so relocs/encodings stay intact), then verify the result equals the target under reloc masking."""
    import re
    man = json.load(open("manifest/functions.json"))["functions"]
    f = next((x for x in man if x["name"] == name), None)
    name, res = solve(name)
    if not isinstance(res, dict):
        return name, f"NOT-SOLVABLE: {res}"
    mapping = res["mapping"]
    tokmap = _token_map(mapping)
    flags = regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq")
    for _ in range(2):
        r = subprocess.run(["bash", "tools/wcc_95.sh", name, flags], capture_output=True)
        if r.returncode == 0 and os.path.exists(f"build/{name}.obj"):
            break
    ob, fx = regdiff.text_bytes_and_fixups(f"build/{name}.obj")
    tb = target_bytes(f)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
    ks = keystone.Ks(keystone.KS_ARCH_X86, keystone.KS_MODE_32)
    rx = re.compile(r"\b(" + "|".join(sorted(tokmap, key=len, reverse=True)) + r")\b")
    out = bytearray()
    for insn in md.disasm(bytes(ob), 0):
        ib = bytes(ob[insn.address:insn.address + insn.size])
        text = (insn.mnemonic + " " + insn.op_str).strip()
        newtext = rx.sub(lambda m: tokmap[m.group(0)], text)
        if newtext == text:
            out += ib                                   # no reg remap -> keep our exact bytes
        else:
            enc, _ = ks.asm(newtext, insn.address)
            if enc is None or len(enc) != insn.size:
                return name, f"REASM-FAIL 0x{insn.address:x}: {text!r} -> {newtext!r} ({None if enc is None else len(enc)}B vs {insn.size})"
            out += bytes(enc)
    ok = M.mask(bytes(out), fx) == M.mask(tb, fx) and len(out) == len(tb)
    return name, ("VERIFIED byte-exact via register-permute " + str(mapping)) if ok else f"MISMATCH after apply (len {len(out)} vs {len(tb)})"

def main():
    global ENC_SWAPS
    import multiprocessing as mp
    if "--enc" in sys.argv:
        ENC_SWAPS = True
    if "--apply" in sys.argv:
        for n in [a for a in sys.argv[1:] if not a.startswith("-")]:
            print("%-28s %s" % apply_and_verify(n), flush=True)
        return
    names = [a for a in sys.argv[1:] if not a.startswith("-")]
    if "--all" in sys.argv:
        man = json.load(open("manifest/functions.json"))["functions"]
        names = [f["name"] for f in man if f.get("status") != "matched"]
    w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 4
    solved = []
    with mp.Pool(w) as pool:                       # compiles are independent -> parallelise
        for name, res in pool.imap_unordered(solve, names):
            if isinstance(res, dict):
                print(f"{name:<28} SOLVABLE  ({res['diff_instrs']} diff, {res['enc_swaps']} enc-swap)  map: {res['mapping']}", flush=True)
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
