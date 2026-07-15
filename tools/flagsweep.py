#!/usr/bin/env python3
"""flagsweep.py -- a PERMUTER for compiler flags (holds the C fixed, sweeps wcc386 flags).

cpermute mutates the source and holds flags fixed; this does the opposite. The original game was
built with ONE flag set per translation unit, which we inferred but never recovered exactly -- and
the recipe distribution shows per-function overfitting (both -3 and -4, plus -d2/-os/-ot). So for a
parked function that won't match, the divergence may be the FLAGS, not the C. This compiles the
unchanged source under a grid of plausible Watcom flag combos in ONE DOSBox/dosemu session (via
wcc95_batch.sh's WCC_FLAGSFILE mode) and scores each with regdiff -- reporting any combo that yields
a byte match, and otherwise the best register-normalised score per combo.

    docker run --rm --device /dev/kvm -e WCC_DOSEMU=1 -v "$PWD":/work -w /work synd-decomp:dosemu \
        python3 tools/flagsweep.py FUN_0002e408         # sweep one function
    ... python3 tools/flagsweep.py --triage              # sweep every parked function, report matches

Grid (default): {-3,-4}x{s,r} CPU/calling  x  optimisation level  x  {frame}  x  {packing}, + `-s -zq`.
"""
import json, os, sys, glob, subprocess
import multiprocessing as mp
import regdiff
from omf import text_bytes_and_fixups

# --- flag search space -------------------------------------------------------------------------
CPU_CALL = ["-3s", "-3r", "-4s", "-4r"]          # 386/486 x stack/register calling
OPT = ["-oneatx", "-oneatx -ol", "-oneatx -oh", "-oneatx -oi", "-oneatx -or",
       "-or", "-os", "-ot", "-ox", ""]           # opt level: full-maxx variants down to reorder/space/time/none
FRAME = ["", "-of"]                              # omit vs force a stack frame
PACK = ["-zp8", "-zp1"]                          # struct packing (8 = default-ish, 1 = tight)
FIXED = "-s -zq"                                 # no stack-overflow check, quiet -- always on


def grid():
    combos = []
    for cc in CPU_CALL:
        for opt in OPT:
            for fr in FRAME:
                for pk in PACK:
                    f = " ".join(" ".join(x for x in (cc, opt, fr, pk, FIXED) if x).split())
                    if f not in combos:
                        combos.append(f)
    return combos


def sweep(name, combos=None):
    combos = combos or grid()
    tb, _ = regdiff.load_target(name)
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return None
    W = f"/tmp/fs_{os.getpid()}"
    os.makedirs(W, exist_ok=True)
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    open(f"{W}/SRC00.C", "w").write(open(sp[0], encoding="utf-8", errors="replace").read())
    open(f"{W}/flags.txt", "w").write("\n".join(combos) + "\n")
    env = dict(os.environ)
    env["WCC_FLAGSFILE"] = f"{W}/flags.txt"
    if os.path.isdir("/tmp/wat"):
        env["WAT_ROOT"] = "/tmp/wat"
    try:
        subprocess.run(["bash", "tools/wcc95_batch.sh", W, "sweep"],
                       capture_output=True, env=env, timeout=900)
    except subprocess.TimeoutExpired:
        return None
    out = []
    for i, fl in enumerate(combos):
        p = f"{W}/O{i:02d}.OBJ"
        if not os.path.exists(p):
            out.append((fl, False, None, "no-compile")); continue
        try:
            ob, fx = text_bytes_and_fixups(p)
        except Exception:
            out.append((fl, False, None, "bad-obj")); continue
        exact = len(tb) == len(ob) and regdiff.mask(tb, fx) == regdiff.mask(ob, fx)
        res = regdiff.analyze(tb, ob, fx)
        out.append((fl, exact, res["score"], res["verdict"]))
    out.sort(key=lambda r: (r[1] is True, r[2] or -1), reverse=True)
    return {"target": len(tb), "n": len(combos), "results": out}


def _worker(name):
    """Pool worker: sweep one function, return (name, top-result-or-None). Each process owns a distinct
    /tmp/fs_<pid> workdir + dosemu session, so N functions sweep concurrently (one core each) -- this is
    what saturates the box; a lone --triage runs one dosemu at a time (~1 core)."""
    try:
        s = sweep(name)
    except Exception:
        return (name, None)
    return (name, s["results"][0] if s else None)


def main():
    if "--triage" in sys.argv:
        workers = (int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv
                   else max(2, min(28, (os.cpu_count() or 4) - 2)))
        man = json.load(open(regdiff.MAN))["functions"]
        SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
        parks = [f for f in man if f.get("status") == "unmatched"
                 and (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC)]
        names = [f["name"] for f in sorted(parks, key=lambda f: f["size"])]
        if os.path.isdir("/work/toolchain/watcom95") and not os.path.isdir("/tmp/wat"):
            subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)   # native compiler tree
        print(f"flag-sweep triage: {len(names)} parked fns x {len(grid())} flag combos, "
              f"{workers} workers", flush=True)
        wins, best_only, done = [], 0, 0
        with mp.Pool(workers) as pool:
            for name, top in pool.imap_unordered(_worker, names):
                done += 1
                if top is None:
                    print(f"[{done}/{len(names)}] {name:<26} SWEEP-FAIL", flush=True); continue
                if top[1]:                               # exact byte match under SOME flag combo
                    wins.append((name, top[0]))
                    print(f"[{done}/{len(names)}] {name:<26} *** BYTE MATCH  under: {top[0]}", flush=True)
                else:
                    best_only += 1
                    print(f"[{done}/{len(names)}] {name:<26} best {top[2]*100:5.1f}% "
                          f"{top[3]:<14} {top[0]}", flush=True)
        print(f"\n=== flag-sweep triage: {len(wins)} newly byte-match under some flag combo, "
              f"{best_only} still no exact ===")
        for n, fl in wins:
            print(f"  {n:<26} {fl}")
        return

    name = sys.argv[1]
    s = sweep(name)
    if not s:
        print(f"{name}: SWEEP-FAIL"); return
    print(f"\n{name}: swept {s['n']} flag combos, target={s['target']}B\n")
    print(f"  {'exact':5} {'score':6} {'verdict':14} flags")
    any_exact = False
    for fl, exact, score, verdict in s["results"][:16]:
        sc = f"{score*100:5.1f}%" if score is not None else "  -  "
        mark = "  ***" if exact else ""
        print(f"  {('YES' if exact else 'no'):5} {sc:6} {verdict:<14} {fl}{mark}")
        any_exact = any_exact or exact
    if any_exact:
        print("\n  *** a flag combo BYTE-MATCHES this function (marked ***) ***")


if __name__ == "__main__":
    main()
