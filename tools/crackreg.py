#!/usr/bin/env python3
"""crackreg.py -- detect parked functions whose ONLY diff vs target is a phantom
callee-saved register save/restore (the "dead callee-save" wall, cracked on 0x36168
via `#pragma aux <callee> modify [reg]`).

For each parked function: compile the current source, disassemble our _TEXT and the
target, normalize every instruction (mask absolute mem displacements and rel branch
targets to a constant), and compare the instruction sequences. Report the class:
  CLEAN-MATCH   sequences identical (already matches modulo reloc -- shouldn't be parked)
  CALLEE-SAVE   target == ours + [push R..] prologue + [..pop R] epilogue, regs R unused
  OTHER         any other structural/allocation difference

Run in-container:
  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/crackreg.py [--only NAME]
"""
import os, sys, json, glob, subprocess
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
FLAGS = "-4s -oneatx -zp8 -s -zq"
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
CSAVE_PUSH = {0x53: "ebx", 0x56: "esi", 0x57: "edi", 0x55: "ebp"}
CSAVE = {"ebx", "esi", "edi", "ebp"}

man = json.load(open("manifest/functions.json"))
IMG = open(SEG, "rb").read()
srcmap = {os.path.basename(p)[:-2]: p for p in glob.glob("src/**/*.c", recursive=True)}


def norm(code, base):
    """Instruction sequence with absolute addresses / branch targets masked."""
    seq = []
    for ins in md.disasm(code, base):
        ops = ins.op_str
        # mask absolute memory displacements  [0x....]
        import re
        ops = re.sub(r"0x[0-9a-f]+", "A", ops)
        seq.append(ins.mnemonic + " " + ops)
    return seq


def target_bytes(addr, size):
    return IMG[addr - 0x10000: addr - 0x10000 + size]


def strip_saves(seq):
    """Remove a matched push R.. / ..pop R callee-save wrapper; return (inner, regs)."""
    regs = []
    i, j = 0, len(seq) - 1
    # trailing ret
    if not seq or seq[-1] != "ret":
        return seq, []
    j -= 1  # point at last non-ret
    # peel leading pushes of callee-saved regs matched by trailing pops (reverse order)
    lead = []
    k = 0
    while k < len(seq) and seq[k].startswith("push ") and seq[k][5:] in CSAVE:
        lead.append(seq[k][5:]); k += 1
    # matching trailing pops in reverse order, just before ret
    t = len(seq) - 2
    tail = []
    while t >= 0 and seq[t].startswith("pop ") and seq[t][4:] in CSAVE:
        tail.append(seq[t][4:]); t -= 1
    # pair them: lead = [r1,r2], tail (reverse) should be [r2,r1]
    paired = []
    tail_rev = list(reversed(tail))
    n = min(len(lead), len(tail_rev))
    m = 0
    while m < n and lead[m] == tail_rev[m]:
        paired.append(lead[m]); m += 1
    if not paired:
        return seq, []
    inner = seq[m: len(seq) - 1 - m] + ["ret"]
    return inner, paired


def compile_fn(name):
    r = subprocess.run(["bash", "tools/wcc_95.sh", name, FLAGS], capture_output=True, text=True)
    if r.returncode != 0:
        return None
    ob, _ = text_bytes_and_fixups(f"build/{name}.obj")
    return ob


def show_diff(name):
    import difflib
    f = by_name_all[name]
    addr, size = int(f["addr"], 16), f.get("size", 0)
    ob = compile_fn(name)
    if ob is None:
        print("COMPILE-FAIL"); return
    ours = norm(ob, addr)
    tgt = norm(target_bytes(addr, size), addr)
    print(f"=== {name} (size {size})  ours={len(ours)} insns  target={len(tgt)} insns ===")
    sm = difflib.SequenceMatcher(a=ours, b=tgt)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i1, i2): print(f"    {ours[k]}")
        else:
            for k in range(i1, i2): print(f"  - {ours[k]}")
            for k in range(j1, j2): print(f"  + {tgt[k]}")


by_name_all = {f["name"]: f for f in man["functions"]}


def main():
    if "--diff" in sys.argv:
        for nm in sys.argv[sys.argv.index("--diff") + 1:]:
            show_diff(nm); print()
        return
    only = None
    if "--only" in sys.argv:
        only = sys.argv[sys.argv.index("--only") + 1]
    parked = [f for f in man["functions"] if f.get("status") != "matched"]
    if only:
        parked = [f for f in parked if f["name"] == only]
    import difflib
    rows = []  # (dist, name, size, kind)
    for f in sorted(parked, key=lambda x: x.get("size", 0)):
        name, addr, size = f["name"], int(f["addr"], 16), f.get("size", 0)
        ob = compile_fn(name)
        if ob is None:
            rows.append((9999, name, size, "COMPILE-FAIL")); continue
        ours = norm(ob, addr)
        tgt = norm(target_bytes(addr, size), addr)
        if ours == tgt:
            rows.append((0, name, size, "CLEAN-MATCH")); continue
        inner, regs = strip_saves(tgt)
        if regs and inner == ours:
            rows.append((0, name, size, "CALLEE-SAVE[%s]" % "+".join(regs))); continue
        # instruction distance = number of non-equal opcodes (both sides)
        sm = difflib.SequenceMatcher(a=ours, b=tgt)
        dist = sum((i2 - i1) + (j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal")
        rows.append((dist, name, size, "OTHER"))
    print("=== parked ranked by instruction-distance (closest first) ===")
    for dist, name, size, kind in sorted(rows):
        print(f"  dist={dist:3d}  {name:24s} size={size:5d}  {kind}")
    print("\n=== closest 20 (best crack candidates) ===")
    for dist, name, size, kind in sorted(rows)[:20]:
        print(f"  {name}:{dist}", end="  ")
    print()

if __name__ == "__main__":
    main()
