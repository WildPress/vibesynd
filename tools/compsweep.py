#!/usr/bin/env python3
"""compsweep.py -- a COMPILER permuter: hold the C + flags fixed, sweep the Watcom VERSION.

The flag sweep (flagsweep.py) proved no flag combo cracks the parked walls -- but it used ONE compiler
(watcom95). We actually have SEVEN period Watcom builds staged in toolchain/, and the walls are
peephole/encoding tie-breaks (imm8-vs-imm32, xor operand choice) -- exactly what shifts between compiler
point releases. This compiles each function with EVERY compiler (its own recipe flags) and reports any
that byte-matches the original where 9.5(base) didn't.

Uses the DOSBox path only (no dosemu/KVM -- that destabilises WSL2). One DOSBox session per (fn,compiler).

    docker run --rm --memory=8g -v "$PWD":/work -w /work synd-decomp \
        python3 tools/compsweep.py FUN_00034048 FUN_00034088 ...     # sweep specific fns
    ... python3 tools/compsweep.py --triage                           # sweep every parked fn
"""
import json, os, sys, glob, subprocess
import multiprocessing as mp
import regdiff
from omf import text_bytes_and_fixups

# name, toolchain tree, DOS WATCOM env, DOS INCLUDE, DOS PATH, host-reldir holding WCC386.EXE.
# A DOSBox-safe DOS4GW (from 9.5b) is dropped into the exe dir -- some trees (watcom10a) ship a
# DOS4GW that faults under DOSBox (error 1012), so we always substitute the known-good one.
GOOD_DOS4GW = "/work/toolchain/watcom95b/BIN/DOS4GW.EXE"
COMPILERS = [
    ("9.01",   "toolchain/watcom901", "C:\\",       "C:\\H",         "C:\\BIN",             "BIN"),
    ("9.5",    "toolchain/watcom95",  "C:\\",       "C:\\H",         "C:\\BIN",             "BIN"),
    ("9.5a",   "toolchain/watcom95a", "C:\\",       "C:\\H",         "C:\\BIN",             "BIN"),
    ("9.5b",   "toolchain/watcom95b", "C:\\",       "C:\\H",         "C:\\BIN",             "BIN"),
    ("9.5c",   "toolchain/watcom95c", "C:\\",       "C:\\H",         "C:\\BIN",             "BIN"),
    ("10.0a",  "toolchain/watcom10a", "C:\\WATCOM", "C:\\WATCOM\\H", "C:\\WATCOM\\BINB;C:\\WATCOM\\BIN", "WATCOM/BINB"),
    ("10.0LA", "toolchain/doswork_la","C:\\",       "C:\\H",         "C:\\",                ""),
]


def prep_trees():
    """Stage each compiler tree to native /tmp ONCE (single-threaded), dropping in a DOSBox-safe DOS4GW.
    Must run before the worker pool -- concurrent cp -r of the same tree races and corrupts it."""
    for name, tree, _, _, _, exedir in COMPILERS:
        ct = "/tmp/ct_" + name
        if not os.path.isdir(ct):
            subprocess.run(f"cp -r /work/{tree} {ct}", shell=True)
            subprocess.run(f"cp -f {GOOD_DOS4GW} {ct}/{exedir}/DOS4GW.EXE", shell=True)


def compile_with(comp, workdir, flags):
    """Compile workdir/SRC00.C with one compiler under DOSBox -> workdir/O.OBJ (or None)."""
    name, tree, watcom, include, pathdir, exedir = comp
    ct = "/tmp/ct_" + name
    W = workdir
    subprocess.run(f"rm -f {W}/O.OBJ", shell=True)
    open(f"{W}/GO.BAT", "w").write("wcc386 %s -fo=O.OBJ SRC00.C\r\n" % flags)
    conf = f"""[cpu]
core=auto
cycles=max
[autoexec]
mount c /tmp/ct_{name}
mount d {W}
set WATCOM={watcom}
set INCLUDE={include}
set DOS4G=quiet
set PATH={pathdir};Z:\\
d:
call D:\\GO.BAT
exit
"""
    open(f"{W}/dbx.conf", "w").write(conf)
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
    try:
        subprocess.run(["dosbox", "-conf", f"{W}/dbx.conf"], capture_output=True, env=env, timeout=120)
    except subprocess.TimeoutExpired:
        return None
    p = f"{W}/O.OBJ"
    return p if os.path.exists(p) else None


def sweep(name):
    tb, _ = regdiff.load_target(name)
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return None
    flags = regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq")
    W = f"/tmp/cs_{os.getpid()}"
    os.makedirs(W, exist_ok=True)
    open(f"{W}/SRC00.C", "w").write(open(sp[0], encoding="utf-8", errors="replace").read())
    out = []
    for comp in COMPILERS:
        obj = compile_with(comp, W, flags)
        if not obj:
            out.append((comp[0], None, "no-compile")); continue
        try:
            ob, fx = text_bytes_and_fixups(obj)
        except Exception:
            out.append((comp[0], None, "bad-obj")); continue
        exact = len(ob) == len(tb) and regdiff.mask(tb, fx) == regdiff.mask(ob, fx)
        res = regdiff.analyze(tb, ob, fx)
        out.append((comp[0], exact, res["verdict"] + " %.0f%%" % (res["score"] * 100)))
    return {"flags": flags, "results": out}


def _worker(name):
    try:
        return (name, sweep(name))
    except Exception:
        return (name, None)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if "--triage" in sys.argv:
        man = json.load(open(regdiff.MAN))["functions"]
        SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
        names = [f["name"] for f in sorted(man, key=lambda f: f["size"])
                 if f.get("status") == "unmatched" and (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC)]
    else:
        names = args
    workers = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 6
    if "--workers" in sys.argv:                          # don't let the workers VALUE be read as a fn name
        names = [n for n in names if n != sys.argv[sys.argv.index("--workers") + 1]]
    print(f"compiler sweep: {len(names)} fn(s) x {len(COMPILERS)} compilers, {workers} workers", flush=True)
    prep_trees()                                         # stage all compiler trees BEFORE the pool (no race)

    wins = []
    with mp.Pool(workers) as pool:
        for name, s in pool.imap_unordered(_worker, names):
            if not s:
                print(f"{name:<26} SWEEP-FAIL", flush=True); continue
            matched = [c for c, ex, v in s["results"] if ex]
            tag = ("*** MATCH: " + ",".join(matched)) if matched else "no match"
            detail = "  ".join(f"{c}:{'YES' if ex else (v.split()[0][:4] if v else '--')}"
                               for c, ex, v in s["results"])
            print(f"{name:<26} {tag:<22} [{detail}]", flush=True)
            if matched:
                wins.append((name, matched, s["flags"]))
    print(f"\n=== compiler sweep: {len(wins)} fn(s) byte-match under a NON-default compiler ===")
    for n, cs, fl in wins:
        print(f"  {n:<26} {','.join(cs)}   ({fl})")


if __name__ == "__main__":
    main()
