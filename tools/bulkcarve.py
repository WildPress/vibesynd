#!/usr/bin/env python3
"""bulkcarve.py -- discover the previously-uncarved function sub-graph that the recovered prefix
(and its main loop FUN_0000d928) reaches, and size each so it can be db-transcribed into the build.

CONTEXT: the decomp was built on the prefix-less linear.bin, so functions reachable ONLY through the
prefix were never carved (verified missing: 0x34d48, 0x20ef8, 0x22e38, 0x29fc8, 0x22cc8, 0x22728,
0x356c8, 0x39158, ...). They must be added before the game can run. Their BYTES already exist in
linear.bin (they are >= 0x10000); only their (addr,size) are unknown -> this tool computes them.

METHOD (in-container, coordinate-clean; bytes from build/obj1_full.bin @ base 0xd748):
  * anchors = every manifest function's [addr, addr+size) (ground-truth built code) + prefix seeds.
  * recursive-descent disasm from every anchor entry, following direct call/jmp/jcc, collects all
    direct call targets.
  * a call target is MISSING if it is not the start of, and not inside, any manifest function
    (i.e. it lands in a gap) -> a real uncarved function entry. Recurse from it too (closure).
  * size(missing) = (next known start above it) - entry, where known starts = manifest starts +
    all missing entries. Tiling against anchors keeps sizes gap-safe (trailing padding absorbed).

OUTPUT: build/missing_fns.json = [{addr,size,name}] for the uncarved sub-graph (>= 0x10000 only;
the prefix [0xd748,0x10000) is handled wholesale by tools/prefix_obj.py). Also prints a summary.

  python3 tools/bulkcarve.py
"""
import json, os, sys, bisect, glob
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET, x86_const

FULL = "build/obj1_full.bin"
MAN  = "manifest/functions.json"
FBASE = 0xd748                    # obj1_full.bin base (manifest coords)
CODE_LO, CODE_HI = 0x10000, 0x4fdf4     # only carve MAIN-BODY gaps here (prefix handled elsewhere)
PREFIX_LO = 0xd748

_md = Cs(CS_ARCH_X86, CS_MODE_32); _md.detail = True
X86_OP_IMM = x86_const.X86_OP_IMM


def main():
    if not os.path.exists(FULL):
        sys.exit("missing %s -- run tools/linearize.py first" % FULL)
    img = open(FULL, "rb").read()
    def at(a):  # bytes of image starting at manifest addr a
        return img[a - FBASE:]

    man = json.load(open(MAN))["functions"]
    starts = sorted(int(f["addr"], 16) for f in man)
    def is_manifest_start(a):
        i = bisect.bisect_left(starts, a)
        return i < len(starts) and starts[i] == a
    have_src = set(os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True))
    def has_src(a):
        return ("FUN_%08x" % a) in have_src

    # seeds: all manifest entries (to reach the gaps) + the prefix entries (to enter the sub-graph)
    PREFIX_SEEDS = [0xd758,0xd928,0xdaa8,0xdc08,0xe568,0xe5a8,0xf5e8,0xf898,
                    0xfa18,0xfa88,0xfb48,0xfd38,0xfee8,0xffc8]
    seeds = list(starts) + PREFIX_SEEDS
    missing = set()
    seen_fn = set()
    work = list(seeds)

    def scan(entry):
        """disasm one function from entry, following intra jcc/jmp for coverage. Returns the set of
        CALL targets only (real function entries). jmp/jcc targets are followed but NOT promoted --
        they are intra-function blocks, not functions. A `jmp imm32` whose target lies OUTSIDE the
        function's own reachable span is a tail-call -> also a function entry."""
        calls = set()
        blocks = [entry]; seen = set(); span_lo = span_hi = entry
        while blocks:
            a = blocks.pop()
            if a in seen: continue
            code = at(a)
            for insn in _md.disasm(code, a):
                if insn.address in seen: break
                seen.add(insn.address)
                span_lo = min(span_lo, insn.address); span_hi = max(span_hi, insn.address + insn.size)
                g = insn.groups
                is_call = CS_GRP_CALL in g
                is_jump = CS_GRP_JUMP in g
                for op in insn.operands:
                    if op.type == X86_OP_IMM and insn.encoding.imm_size == 4:
                        v = op.imm & 0xFFFFFFFF
                        if not (PREFIX_LO <= v <= CODE_HI):
                            continue
                        if is_call:
                            calls.add(v)                          # function entry
                        elif is_jump:
                            # near intra-fn branch: follow. far jmp to a manifest start: tail-call.
                            if is_manifest_start(v) and not (entry - 0x40 <= v <= span_hi + 0x800):
                                calls.add(v)
                            else:
                                blocks.append(v)
                if CS_GRP_RET in g:
                    break
                if is_jump and not is_call and insn.mnemonic == "jmp":
                    break
        return calls

    while work:
        e = work.pop()
        if e in seen_fn: continue
        seen_fn.add(e)
        for t in scan(e):
            if not (CODE_LO <= t < CODE_HI):
                continue
            if is_manifest_start(t) or has_src(t):
                if t not in seen_fn: work.append(t)      # already built: recurse to reach deeper gaps
                continue
            # called, in-range, no manifest entry, no source -> a genuinely uncarved function
            # (drop the old in-a-manifest-range guard: oversized manifest ranges were HIDING real fns)
            if t not in missing:
                missing.add(t); work.append(t)

    # size each missing fn = next known start above it (manifest starts + missing) - entry
    known = sorted(set(starts) | missing)
    out = []
    for a in sorted(missing):
        i = bisect.bisect_right(known, a)
        end = known[i] if i < len(known) else CODE_HI
        out.append({"addr": "%08x" % a, "size": end - a, "name": "FUN_%08x" % a})
    os.makedirs("build", exist_ok=True)
    json.dump(out, open("build/missing_fns.json", "w"), indent=0)
    print("MISSING sub-graph functions (>= 0x10000): %d" % len(out))
    for f in out:
        print("  %s  size=%d" % (f["addr"], f["size"]))


if __name__ == "__main__":
    main()
