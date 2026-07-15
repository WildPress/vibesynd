#!/usr/bin/env python3
"""classify_equiv.py -- split the parked functions into "equivalent" (same instructions, only a
register/encoding tie-break differs -> behaviourally COMPLETE, just not byte-identical) vs genuinely
"structural" (a real instruction-sequence difference remains). Compiles each with both calling
conventions (-4s and -4r) plus any header recipe, and keeps the BEST regdiff verdict, so wrong-flag
noise doesn't mislabel a true equivalent as structural.

Writes manifest/equivalence.json { name: {verdict, flags, score} }. treemap.py / progress.py read it
to colour the "equivalent" tier blue. Run in-container (needs the period compiler + capstone):
    docker run --rm --memory=8g -v "$PWD":/work -w /work synd-decomp python3 tools/classify_equiv.py
"""
import os, sys, glob, json
import multiprocessing as mp
import regdiff

RANK = {"MATCH": 3, "PURE-ALLOC": 2, "REGISTER-ROLE": 2, "STRUCTURAL": 1}
EQUIV = {"MATCH", "PURE-ALLOC", "REGISTER-ROLE"}     # same instruction sequence -> behaviourally complete


def classify(name):
    tb, _ = regdiff.load_target(name)
    flagsets = []
    for f in (regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq"),
              "-4s -oneatx -zp8 -s -zq", "-4r -oneatx -zp8 -s -zq"):
        if f not in flagsets:
            flagsets.append(f)
    best = None
    for fl in flagsets:
        c = regdiff.compile_one(name, fl)
        if not c:
            continue
        res = regdiff.analyze(tb, c[0], c[1])
        key = (RANK.get(res["verdict"], 0), res["score"])
        if best is None or key > best[0]:
            best = (key, {"verdict": res["verdict"], "flags": fl, "score": round(res["score"], 4)})
    return name, (best[1] if best else None)


def main():
    man = json.load(open(regdiff.MAN))["functions"]
    SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
    names = [f["name"] for f in man if f.get("status") == "unmatched"
             and (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC)]
    w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 6
    print(f"classifying {len(names)} parked fns (both -4s/-4r), {w} workers", flush=True)
    out, n_eq, n_st = {}, 0, 0
    with mp.Pool(w) as pool:
        for i, (name, res) in enumerate(pool.imap_unordered(classify, names), 1):
            out[name] = res
            eq = res and res["verdict"] in EQUIV
            n_eq += eq
            n_st += bool(res) and not eq
            tag = "EQUIVALENT" if eq else (res["verdict"] if res else "COMPILE-FAIL")
            print(f"[{i}/{len(names)}] {name:<26} {tag}"
                  + (f"  ({res['verdict']} {res['score']*100:.0f}%, {res['flags']})" if res else ""), flush=True)
    json.dump(out, open("manifest/equivalence.json", "w"), indent=0)
    print(f"\n=== {n_eq} EQUIVALENT (register/encoding-only -> complete), {n_st} structural, "
          f"{len(names)-n_eq-n_st} compile-fail ===")
    print("wrote manifest/equivalence.json")


if __name__ == "__main__":
    main()
