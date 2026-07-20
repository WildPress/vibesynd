#!/usr/bin/env python3
"""modify_probe.py -- detect which unmatched call-out functions the callee `#pragma aux modify`
lever can actually move, so we only hand those to a guided sweep.

Background: declaring a CALLEE's real register-clobber contract (`#pragma aux <callee> modify [regs]`)
reshapes the CALLER's register colouring and matched walk_sound_record_table where nothing else could.
But it only bites when a value is live across a call in a callee-saved reg that the original callee
clobbered. A sweep of the 9 closest call-out fns found 8 were scheduler / spill-slot / instruction-
selection ties the lever cannot touch -- high byte-closeness did NOT predict applicability.

The mechanical discriminator (from mission_map_init's triage): if we declare EVERY callee clobbers
EVERYTHING (max perturbation) and the compiled output does NOT change one byte, the allocation is
insensitive to callee contracts -> the lever is provably INERT, skip it. If the output DOES change,
the function is lever-SENSITIVE; if a perturbation also pushes the reloc-aware first-diff LATER, it is
a strong candidate for a guided modify-set sweep.

    docker run --rm --memory=8g -v "$PWD":/work -w /work synd-decomp python3 tools/modify_probe.py
Writes manifest/modify_probe.json { name: {baseline_fd, perturb_fd, changed, callees:[...], promising} }
and prints the SENSITIVE / PROMISING shortlist.
"""
import json, os, sys, re, glob, subprocess
import regdiff as R
from omf import text_bytes_and_fixups

DEFAULT = "-4s -oneatx -zp8 -s -zq"
ALLGP = "eax ecx edx ebx esi edi ebp"          # max clobber (everything but esp)
OUTF = "manifest/modify_probe.json"


def externs(txt):
    """Callee names this TU declares -- every function it calls must be declared to call it, so the
    extern list is the set of modify-set targets. Skip the fn's own name."""
    names = re.findall(r"extern\s+[^;{}]*?\b(\w+)\s*\(", txt)
    return list(dict.fromkeys(names))


def inject(txt, callees, regs):
    """Insert one `#pragma aux <callee> modify [regs];` per callee, right after the last extern line
    (aux pragmas associate by name and must follow the declaration)."""
    lines = txt.splitlines(keepends=True)
    last = 0
    for i, ln in enumerate(lines):
        if "extern" in ln and ";" in ln:
            last = i + 1
    prag = "".join(f"#pragma aux {c} modify [{regs}];\n" for c in callees)
    return "".join(lines[:last]) + prag + "".join(lines[last:])


def compile_text(name, src_text, flags):
    W = f"/tmp/mp_{os.getpid()}_{abs(hash(name)) % 100000}"
    os.makedirs(W, exist_ok=True)
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    open(f"{W}/SRC00.C", "w").write(src_text)
    env = dict(os.environ)
    if os.path.isdir("/tmp/wat"):
        env["WAT_ROOT"] = "/tmp/wat"
    try:
        subprocess.run(["bash", "tools/wcc95_batch.sh", W, flags], capture_output=True, env=env, timeout=90)
    except subprocess.TimeoutExpired:
        return None
    p = f"{W}/O00.OBJ"
    if not os.path.exists(p):
        return None
    try:
        return text_bytes_and_fixups(p)
    except Exception:
        return None


def first_diff(tb, ob, fx):
    """Reloc-aware first-diff offset vs target (masked); None if masked-equal up to min length."""
    tm, om = R.mask(tb, fx), R.mask(ob, fx)
    n = min(len(tm), len(om))
    for i in range(n):
        if tm[i] != om[i]:
            return i
    return n if len(tm) != len(om) else None      # equal prefix; differ only in trailing length


def probe(f):
    name = f["name"]
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return name, None
    txt = open(sp[0], encoding="utf-8", errors="replace").read()
    cs = [c for c in externs(txt) if c != name]
    flags = R.recipe_flags(name, DEFAULT)
    tb, _ = R.load_target(name)
    base = compile_text(name, txt, flags)
    if not base:
        return name, {"error": "baseline compile-fail"}
    bob, bfx = base
    bfd = first_diff(tb, bob, bfx)
    if not cs:
        return name, {"baseline_fd": bfd, "perturb_fd": bfd, "changed": False,
                      "callees": [], "promising": False, "note": "no callees"}
    # (1) INERT check: does clobbering everything on every callee change ANY byte? If not, the
    # function's allocation is insensitive to callee contracts -> lever provably cannot help.
    maxp = compile_text(name, inject(txt, cs, ALLGP), flags)
    if not maxp:
        return name, {"baseline_fd": bfd, "perturb_fd": None, "changed": None,
                      "callees": cs, "promising": False, "note": "perturb compile-fail"}
    changed = (maxp[0] != bob)
    if not changed:
        return name, {"baseline_fd": bfd, "perturb_fd": bfd, "changed": False,
                      "callees": cs, "promising": False}
    # (2) DIRECTION check: the lever can only ADD clobbers (safe). Test adding ONE callee-saved reg
    # at a time to every callee's modify set (walk_sound-style) and keep the one that moves the
    # reloc-aware first-diff the LATEST. If the best beats baseline, the lever has a beneficial
    # direction here -> PROMISING, worth a guided per-callee sweep.
    best_fd, best_reg = bfd, None
    fds = {}
    CS = ("ebx", "esi", "edi", "ebp")
    # add-one: clobber [scratch + R] (force value R OUT); preserve-one: clobber [scratch + the other
    # three] (force a value INTO R -- this is walk_sound's winning shape, preserve ESI only).
    trials = [("+" + r, "eax ecx edx " + r) for r in CS] + \
             [("keep" + r, "eax ecx edx " + " ".join(x for x in CS if x != r)) for r in CS]
    for label, regs in trials:
        c = compile_text(name, inject(txt, cs, regs), flags)
        if not c:
            continue
        fd = first_diff(tb, c[0], c[1])
        fds[label] = fd
        if fd is None:                      # fully masked-equal -> match candidate, stop
            best_fd, best_reg = None, label
            break
        if best_fd is not None and fd > best_fd:
            best_fd, best_reg = fd, label
    promising = (best_reg is not None) and (best_fd is None or (bfd is not None and best_fd > bfd))
    return name, {"baseline_fd": bfd, "perturb_fd": first_diff(tb, maxp[0], maxp[1]),
                  "changed": True, "callees": cs, "promising": promising,
                  "best_add": best_reg, "best_fd": best_fd, "add_fds": fds}


def fine_probe(f):
    """Finer than probe(): sweep EACH callee individually x each callee-saved reg, in both the
    add-one (clobber scratch+R, guarded_init_alloc's shape) and preserve-one (clobber scratch + the
    other three, walk_sound's shape) directions. Catches matches that need a SPECIFIC register on a
    SPECIFIC callee -- which the coarse all-callees probe regresses and misses."""
    name = f["name"]
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return name, None
    txt = open(sp[0], encoding="utf-8", errors="replace").read()
    cs = [c for c in externs(txt) if c != name]
    flags = R.recipe_flags(name, DEFAULT)
    tb, _ = R.load_target(name)
    base = compile_text(name, txt, flags)
    if not base:
        return name, {"error": "baseline compile-fail"}
    bfd = first_diff(tb, base[0], base[1])
    CS = ("ebx", "esi", "edi", "ebp")
    best_fd, best = bfd, None
    match = None
    for c in cs:
        sets = [(f"{c}+{r}", "eax ecx edx " + r) for r in CS] + \
               [(f"{c} keep{r}", "eax ecx edx " + " ".join(x for x in CS if x != r)) for r in CS]
        for label, regs in sets:
            comp = compile_text(name, inject(txt, [c], regs), flags)
            if not comp:
                continue
            fd = first_diff(tb, comp[0], comp[1])
            if fd is None:
                match = label
                break
            if best_fd is not None and fd > best_fd:
                best_fd, best = fd, label
        if match:
            break
    return name, {"baseline_fd": bfd, "best_fd": (None if match else best_fd),
                  "best": (match or best), "match_candidate": bool(match)}


def main():
    import multiprocessing as mp
    man = json.load(open("manifest/functions.json"))["functions"]
    if "--fine" in sys.argv:
        # default target set: the SENSITIVE-not-inert fns from a prior coarse run
        try:
            prev = json.load(open(OUTF))
            names = [k for k, v in prev.items() if v and v.get("changed")]
        except Exception:
            names = []
        if "--only" in sys.argv:
            names = sys.argv[sys.argv.index("--only") + 1].split(",")
        fns = [f for f in man if f["name"] in set(names) and f.get("status") != "matched"]
        w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 6
        print(f"FINE per-callee x per-reg probe over {len(fns)} fns, {w} workers", flush=True)
        out = {}
        with mp.Pool(w) as pool:
            for i, (name, res) in enumerate(pool.imap_unordered(fine_probe, fns), 1):
                out[name] = res
                tag = ("MATCH-CANDIDATE via " + res["best"]) if res and res.get("match_candidate") else (
                    f"best_fd=0x{res['best_fd']:x} via {res['best']}" if res and res.get("best") else
                    (res.get("error", res.get("note", "no-improve")) if res else "?"))
                print(f"[{i:2}/{len(fns)}] {name:<30} base=0x{(res.get('baseline_fd') or 0):x}  {tag}", flush=True)
        json.dump(out, open("manifest/modify_probe_fine.json", "w"), indent=0)
        mc = [(k, v) for k, v in out.items() if v and v.get("match_candidate")]
        print(f"\nwrote manifest/modify_probe_fine.json")
        print(f"\n=== FINE MATCH-CANDIDATES (a single-callee modify set reaches masked-equal) : {len(mc)} ===")
        for k, v in mc:
            print(f"  {k:<30} via  #pragma aux {v['best'].split()[0]} modify [...]  ({v['best']})")
        return
    call_out = [f for f in man if f.get("status") != "matched" and f.get("calls", 0) > 0
                and glob.glob(f"src/**/{f['name']}.c", recursive=True)]
    if "--only" in sys.argv:
        want = set(sys.argv[sys.argv.index("--only") + 1].split(","))
        call_out = [f for f in call_out if f["name"] in want]
    w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 4
    print(f"probing {len(call_out)} unmatched call-out fns (baseline vs max-clobber), {w} workers", flush=True)
    out = {}
    with mp.Pool(w) as pool:
        for i, (name, res) in enumerate(pool.imap_unordered(probe, call_out), 1):
            out[name] = res
            tag = "?" if not res else (
                "SENSITIVE" + (" +PROMISING" if res.get("promising") else "") if res.get("changed")
                else "inert" if res.get("changed") is False else res.get("note", res.get("error", "?")))
            print(f"[{i:2}/{len(call_out)}] {name:<30} base_fd={res.get('baseline_fd') if res else '?'} "
                  f"pert_fd={res.get('perturb_fd') if res else '?'}  {tag}", flush=True)
    json.dump(out, open(OUTF, "w"), indent=0)
    sens = [(k, v) for k, v in out.items() if v and v.get("changed")]
    prom = [(k, v) for k, v in sens if v.get("promising")]
    print(f"\nwrote {OUTF}")
    print(f"\n=== PROMISING (adding a callee-saved reg to the clobber set moves first-diff LATER) "
          f": {len(prom)} ===")
    for k, v in sorted(prom, key=lambda kv: -( (kv[1].get('best_fd') or 1<<30) )):
        bf = "MATCH-CANDIDATE" if v.get("best_fd") is None else f"0x{v['best_fd']:x}"
        print(f"  {k:<30} base_fd=0x{(v['baseline_fd'] or 0):x} -> {bf} via +{v.get('best_add')} "
              f"({len(v['callees'])} callees)")
    print(f"\n=== SENSITIVE-but-not-promising (output moves but no beneficial add) : {len(sens)-len(prom)} ===")
    for k, v in sorted([s for s in sens if not s[1]['promising']], key=lambda kv: kv[1]['baseline_fd'] or 0):
        print(f"  {k:<30} base_fd=0x{(v['baseline_fd'] or 0):x}  callees={len(v['callees'])}")
    print(f"\n=== INERT (perturbation is a no-op -> lever cannot help, skip) : "
          f"{sum(1 for k,v in out.items() if v and v.get('changed') is False)} ===")


if __name__ == "__main__":
    main()
