#!/usr/bin/env python3
"""regdiff.py -- REGISTER-NORMALISED diff of our compiled function vs the original.

The permuter's byte-difflib score is a poor gradient for our dominant wall class: a single
register-role or spill-slot difference perturbs every modrm byte that uses it, so "one register
transposed" (structurally perfect) scores almost the same as "logically wrong". This tool instead
decodes BOTH functions to instructions and compares them modulo:
  * relocations  -- both byte strings are pre-masked at our object's fixup ranges, so link-address
                    and call/global fields read equal;
  * register renaming -- a consistent bijection between physical registers;
  * stack-slot renaming -- a consistent bijection between esp/ebp-relative displacements.

It then classifies the divergence and, when it is pure allocation, prints the exact map:
  MATCH                 masked bytes already identical
  PURE-ALLOC            same instructions, only register/slot assignment differs  (report the map)
                          - stack-slot transpose   (only esp/ebp disps differ)
                          - register renaming       (only registers differ)
  REGISTER-ROLE         same instructions but the register mapping is NOT a bijection (a value the
                        target keeps in one register we spread across two, etc.) -- report conflicts
  STRUCTURAL            different instruction count, or a mnemonic/operand-shape mismatch -- report
                        the first diverging instruction (this is real logic/codegen divergence)

This turns blind grinding into a verdict: PURE-ALLOC with no C lever to force it == a genuine wall
(stop hand-grinding); STRUCTURAL == there is still a source change to find. It is also the clean
gradient the annealer wants, and the crisp diagnosis an agent-in-the-loop can act on.

    docker run --rm -v "$PWD":/work -w /work synd-decomp \
        python3 tools/regdiff.py <name> "<wcc386 flags>"        # compile src, diff, verdict
    python3 tools/regdiff.py --triage "<flags>"                  # verdict for every parked function
"""
import json, sys, os, glob, subprocess, re
from omf import text_bytes_and_fixups
try:
    import capstone
except ImportError:
    subprocess.run("pip install -q --break-system-packages capstone", shell=True)
    import capstone

SEG, MAN = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "manifest/functions.json"
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True
X = capstone.x86


def mask(b, fx):
    b = bytearray(b)
    for off, size in fx:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)


def recipe_flags(name, default):
    """Each parked function has its OWN intended flags -- compiling with the wrong ones invents
    structural noise. Prefer manifest/recipes.json (matched fns), else the `Recipe:` line the source
    header records, else the caller's default."""
    try:
        rec = json.load(open("manifest/recipes.json"))
        if name in rec and rec[name].get("flags"):
            return rec[name]["flags"]
    except Exception:
        pass
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if sp:
        head = open(sp[0], encoding="utf-8", errors="replace").read()[:2500]
        m = re.search(r"Recipe:\s*(-[^\n*;]+)", head)
        if m:
            return m.group(1).strip()
    return default


def compile_one(name, flags):
    """Compile src/**/<name>.c with the period Watcom compiler (one DOSBox batch) -> (.text, fixups)."""
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return None
    W = f"/tmp/rd_{os.getpid()}"
    os.makedirs(W, exist_ok=True)
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    open(f"{W}/SRC00.C", "w").write(open(sp[0], encoding="utf-8", errors="replace").read())
    env = dict(os.environ)
    if os.path.isdir("/tmp/wat"):
        env["WAT_ROOT"] = "/tmp/wat"
    try:
        subprocess.run(["bash", "tools/wcc95_batch.sh", W, flags], capture_output=True, env=env,
                       timeout=90)                       # dosemu can wedge; don't hang the triage
    except subprocess.TimeoutExpired:
        return None
    p = f"{W}/O00.OBJ"
    if not os.path.exists(p):
        return None
    try:
        return text_bytes_and_fixups(p)
    except Exception:
        return None


def toks(insn):
    """(mnemonic, shape, regs, slots) for one instruction.
    shape = operand structure with registers->'R', stack disps->'S' (so structurally-identical
            instructions with different allocation compare equal);
    regs  = the actual physical registers, in operand order (for the renaming bijection);
    slots = the actual esp/ebp-relative displacements, in order (for the slot bijection)."""
    shape, regs, slots = [], [], []
    is_branch = insn.group(capstone.CS_GRP_JUMP)      # jcc/jmp: imm is a RELATIVE target
    for op in insn.operands:
        if op.type == X.X86_OP_REG:
            shape.append("R"); regs.append(insn.reg_name(op.reg))
        elif op.type == X.X86_OP_IMM:
            # a branch displacement differs whenever anything DOWNSTREAM changes size -- it's
            # layout-driven, not logic, so mask it (like a relocation) instead of calling it a diff.
            shape.append(("imm", "~rel") if is_branch else ("imm", op.imm))
        elif op.type == X.X86_OP_MEM:
            m = op.mem
            base = insn.reg_name(m.base) if m.base else None
            idx = insn.reg_name(m.index) if m.index else None
            if base in ("esp", "ebp"):                     # stack local -> renameable slot
                shape.append(("mem", "R" if base else None, idx, m.scale, "S"))
                regs.append(base); slots.append(m.disp)
                if idx:
                    regs.append(idx)
            else:
                if base:
                    regs.append(base)
                if idx:
                    regs.append(idx)
                shape.append(("mem", "R" if base else None, "R" if idx else None, m.scale, m.disp))
        else:
            shape.append(("?",))
    return (insn.mnemonic, tuple(shape), tuple(regs), tuple(slots))


def analyze(tb, ob, fx):
    import difflib
    tbm, obm = mask(tb, fx), mask(ob, fx)
    ti = [toks(i) for i in MD.disasm(tbm, 0)]
    oi = [toks(i) for i in MD.disasm(obm, 0)]
    if tbm == obm and len(tb) == len(ob):
        return {"verdict": "MATCH", "score": 1.0, "n": len(ti)}

    # Align by STRUCTURAL key (mnemonic + shape, with registers/slots/branch-targets abstracted out).
    # difflib finds inserted/deleted instructions, so a count mismatch pinpoints the real edit instead
    # of cascading into every downstream branch displacement.
    tk = [(t[0], t[1]) for t in ti]
    ok = [(o[0], o[1]) for o in oi]
    sm = difflib.SequenceMatcher(None, tk, ok, autojunk=False)
    ops = sm.get_opcodes()
    score = sm.ratio()

    if not all(tag == "equal" for tag, *_ in ops):         # a real structural edit exists
        for tag, i1, i2, j1, j2 in ops:
            if tag != "equal":
                return {"verdict": "STRUCTURAL", "at": i1, "tag": tag,
                        "t": ti[i1] if i1 < len(ti) else None,
                        "o": oi[j1] if j1 < len(oi) else None,
                        "score": score, "n": max(len(ti), len(oi))}

    # structurally identical -> the ONLY differences are register / stack-slot assignment.
    regmap, slotmap = {}, {}
    reg_conflict, slot_conflict = [], []
    for t, o in zip(ti, oi):
        for a, b in zip(t[2], o[2]):                       # register bijection
            if regmap.get(a, b) != b:
                reg_conflict.append((a, regmap[a], b))
            regmap[a] = b
        for a, b in zip(t[3], o[3]):                       # stack-slot bijection
            if slotmap.get(a, b) != b:
                slot_conflict.append((a, slotmap[a], b))
            slotmap[a] = b

    reg_moved = {a: b for a, b in regmap.items() if a != b}
    slot_moved = {a: b for a, b in slotmap.items() if a != b}
    # bijection check: no two target regs map to the same ours, no conflicts recorded
    bij = (not reg_conflict and not slot_conflict
           and len(set(regmap.values())) == len(regmap)
           and len(set(slotmap.values())) == len(slotmap))
    if bij:
        sub = ("stack-slot transpose" if slot_moved and not reg_moved else
               "register renaming" if reg_moved and not slot_moved else
               "register + slot renaming")
        return {"verdict": "PURE-ALLOC", "sub": sub, "regmap": reg_moved, "slotmap": slot_moved,
                "score": 1.0, "n": len(ti)}
    return {"verdict": "REGISTER-ROLE", "reg_conflict": reg_conflict[:4],
            "slot_conflict": slot_conflict[:4], "regmap": reg_moved, "slotmap": slot_moved,
            "score": score, "n": len(ti)}


def load_target(name):
    man = json.load(open(MAN))
    base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    return open(SEG, "rb").read()[off:off + f["size"]], f


def report(name, res):
    v = res["verdict"]
    hdr = f"{name:<26} {v:<14} {res['score']*100:5.1f}% struct  n={res.get('n','?')}"
    print(hdr)
    if v == "PURE-ALLOC":
        print(f"    {res['sub']}")
        if res["regmap"]:
            print("    registers:  " + ", ".join(f"{a}->{b}" for a, b in res["regmap"].items()))
        if res["slotmap"]:
            print("    slots:      " + ", ".join(f"[+{a:#x}]->[+{b:#x}]" for a, b in res["slotmap"].items()))
        print("    => only allocation differs; no C spelling forces this -> genuine wall "
              "(unless #pragma aux can pin it).")
    elif v == "REGISTER-ROLE":
        print(f"    non-bijective register use (a value the target keeps in one reg, we split):")
        if res.get("regmap"):
            print("    reg map:    " + ", ".join(f"{a}->{b}" for a, b in res["regmap"].items()))
        if res.get("slotmap"):
            print("    slot map:   " + ", ".join(f"[+{a:#x}]->[+{b:#x}]" for a, b in res["slotmap"].items()))
        for a, was, now in res.get("reg_conflict", []):
            print(f"      collision: target {a} mapped to both {was} and {now}")
        for a, was, now in res.get("slot_conflict", []):
            print(f"      collision: target [+{a:#x}] mapped to both [+{was:#x}] and [+{now:#x}]")
        print("    => allocation-internal; usually a wall, occasionally movable by forcing a temp/spill.")
    elif v == "STRUCTURAL":
        tag = res.get("tag", "replace")
        kind = {"replace": "differs", "insert": "target has an EXTRA instruction",
                "delete": "ours has an EXTRA instruction"}.get(tag, tag)
        print(f"    first structural edit at target instr {res['at']} ({kind}):")
        if res.get("t"):
            print(f"      target: {res['t'][0]} {res['t'][1]}")
        if res.get("o"):
            print(f"      ours:   {res['o'][0]} {res['o'][1]}")
        print("    => real codegen/logic divergence: there IS a source change to find here.")


def show(name, flags):
    """Print the target vs ours disassembly, aligned by structural key, divergences marked with '|'.
    This is the hand-crafting view: compile src/<name>.c, decode both (masked), align, show side by side
    so a human can read off exactly which instruction/register differs and reshape the C toward it."""
    import difflib
    tb, _ = load_target(name)
    c = compile_one(name, flags)
    if not c:
        print(f"{name}: COMPILE-FAIL"); return
    ob, fx = c
    tbm, obm = mask(tb, fx), mask(ob, fx)
    ti = list(MD.disasm(tbm, 0)); oi = list(MD.disasm(obm, 0))
    tk = [(i.mnemonic, toks(i)[1]) for i in ti]
    ok = [(i.mnemonic, toks(i)[1]) for i in oi]
    sm = difflib.SequenceMatcher(None, tk, ok, autojunk=False)
    def fmt(i): return f"{i.mnemonic} {i.op_str}" if i else ""
    print(f"\n{name}  target={len(tb)}B ours={len(ob)}B  ({'MATCH' if tbm==obm and len(tb)==len(ob) else 'diff'})")
    print(f"  {'TARGET (original)':<34}{'OURS (our C -> 9.5b)':<34}")
    print("  " + "-" * 70)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        n = max(i2 - i1, j2 - j1)
        for k in range(n):
            t = ti[i1 + k] if i1 + k < i2 else None
            o = oi[j1 + k] if j1 + k < j2 else None
            mark = " " if tag == "equal" else "|"
            print(f"  {fmt(t):<34}{mark} {fmt(o):<34}")


def main():
    if "--show" in sys.argv:
        i = sys.argv.index("--show")
        show(sys.argv[i + 1], sys.argv[i + 2] if i + 2 < len(sys.argv) else "-4s -oneatx -zp8 -s -zq")
        return
    if "--triage" in sys.argv:
        flags = sys.argv[sys.argv.index("--triage") + 1]
        man = json.load(open(MAN))["functions"]
        SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
        parks = [f for f in man if f.get("status") == "unmatched"
                 and (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC)]
        tally = {}
        for f in sorted(parks, key=lambda f: f["size"]):
            tb, _ = load_target(f["name"])
            c = compile_one(f["name"], recipe_flags(f["name"], flags))
            if not c:
                print(f"{f['name']:<26} COMPILE-FAIL"); tally["COMPILE-FAIL"] = tally.get("COMPILE-FAIL", 0)+1
                continue
            res = analyze(tb, c[0], c[1])
            report(f["name"], res)
            tally[res["verdict"]] = tally.get(res["verdict"], 0) + 1
        print("\n=== triage tally ===")
        for k, v in sorted(tally.items(), key=lambda kv: -kv[1]):
            print(f"  {k:<14} {v}")
        return

    name, flags = sys.argv[1], sys.argv[2]
    tb, _ = load_target(name)
    c = compile_one(name, flags)
    if not c:
        print(f"{name}: COMPILE-FAIL"); return
    report(name, analyze(tb, c[0], c[1]))


if __name__ == "__main__":
    main()
