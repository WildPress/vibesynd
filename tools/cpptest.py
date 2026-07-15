#!/usr/bin/env python3
"""cpptest.py -- compile a function's C source with the Watcom C++ compiler (wpp386) instead of the
C compiler (wcc386), and diff vs the original. Tests the hypothesis that the game was built through
wpp386 (whose register allocator can differ from wcc386's) -- a lever the whole campaign never tried.

    docker run --rm --memory=4g -v "$PWD":/work -w /work synd-decomp python3 tools/cpptest.py FUN_00038c28
"""
import os, sys, glob, subprocess, json
import regdiff
from omf import text_bytes_and_fixups

# 9.5b (the game's confirmed version) with wpp386 (C++, extracted from the download) vs wcc386 (C).
# .CPP forces C++ mode; the C control compiles the SAME source as C. This is the clean test.
def compile_cpp(name, flags, cc="wpp386"):
    sp = glob.glob(f"src/**/{name}.c", recursive=True)
    if not sp:
        return None
    if not os.path.isdir("/tmp/w95bcpp"):
        subprocess.run("cp -r /work/toolchain/watcom95b /tmp/w95bcpp", shell=True)
    W = f"/tmp/cpp_{os.getpid()}_{cc}"
    os.makedirs(W, exist_ok=True)
    subprocess.run(f"rm -f {W}/O.OBJ", shell=True)
    src = ("SRC00.CPP" if cc == "wpp386" else "SRC00.C")           # .CPP forces C++ mode
    open(f"{W}/{src}", "w").write(open(sp[0], encoding="utf-8", errors="replace").read())
    open(f"{W}/GO.BAT", "w").write("%s %s -fo=O.OBJ %s > OUT.TXT\r\n" % (cc, flags, src))
    open(f"{W}/dbx.conf", "w").write(
        "[cpu]\ncore=auto\ncycles=max\n[autoexec]\n"
        "mount c /tmp/w95bcpp\nmount d %s\n"
        "set WATCOM=C:\\\nset INCLUDE=C:\\H\nset DOS4G=quiet\nset PATH=C:\\BIN;Z:\\\n"
        "d:\ncall D:\\GO.BAT\nexit\n" % W)
    subprocess.run(["dosbox", "-conf", f"{W}/dbx.conf"],
                   capture_output=True, env=dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy"),
                   timeout=120)
    p = f"{W}/O.OBJ"
    if not os.path.exists(p):
        err = open(f"{W}/OUT.TXT").read() if os.path.exists(f"{W}/OUT.TXT") else "(no output)"
        print(f"{cc} did not produce an object. compiler said:\n" + err[:600])
        return None
    return text_bytes_and_fixups(p)


def _worker(name):
    try:
        flags = regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq")
        tb, _ = regdiff.load_target(name)
        c = compile_cpp(name, flags, "wpp386")
        if not c:
            return (name, None)
        ob, fx = c
        exact = len(ob) == len(tb) and regdiff.mask(tb, fx) == regdiff.mask(ob, fx)
        return (name, ("MATCH" if exact else regdiff.analyze(tb, ob, fx)["verdict"]))
    except Exception:
        return (name, None)


def main():
    if "--triage" in sys.argv:
        import multiprocessing as mp
        man = json.load(open(regdiff.MAN))["functions"]
        SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
        names = [f["name"] for f in sorted(man, key=lambda f: f["size"])
                 if f.get("status") == "unmatched" and (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC)]
        subprocess.run("cp -r /work/toolchain/watcom95b /tmp/w95bcpp", shell=True)   # stage once, no race
        w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 6
        print(f"C++ (9.5b wpp386) triage: {len(names)} parked fns, {w} workers", flush=True)
        wins = []
        with mp.Pool(w) as pool:
            for i, (name, v) in enumerate(pool.imap_unordered(_worker, names), 1):
                if v == "MATCH":
                    wins.append(name); print(f"[{i}/{len(names)}] {name} *** C++ BYTE MATCH ***", flush=True)
                else:
                    print(f"[{i}/{len(names)}] {name:<26} {v}", flush=True)
        print(f"\n=== {len(wins)} parked fns byte-match as C++ (9.5b wpp386) ===")
        for n in wins:
            print(" ", n)
        return

    name = sys.argv[1]
    flags = sys.argv[2] if len(sys.argv) > 2 else regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq")
    tb, _ = regdiff.load_target(name)
    print(f"{name}  (9.5b -- the game's version; a C++ MATCH here would be REAL)")
    objs = {}
    for cc in ("wcc386", "wpp386"):
        c = compile_cpp(name, flags, cc)
        if not c:
            continue
        objs[cc] = c
        ob, fx = c
        exact = len(ob) == len(tb) and regdiff.mask(tb, fx) == regdiff.mask(ob, fx)
        res = regdiff.analyze(tb, ob, fx)
        print(f"  {cc:<7} ({'C' if cc=='wcc386' else 'C++'}): {len(ob)}B  "
              + ("*** BYTE MATCH ***" if exact else f"{res['verdict']} {res['score']*100:.1f}%"))
    if "wcc386" in objs and "wpp386" in objs:
        same = objs["wcc386"][0] == objs["wpp386"][0]
        print("  => C and C++ compile to " + ("IDENTICAL bytes (C++ lever is inert here)"
              if same else "DIFFERENT bytes -- C-vs-C++ IS a real codegen lever; extract 9.5's wpp386"))


if __name__ == "__main__":
    main()
