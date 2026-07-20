#!/usr/bin/env python3
"""flag_sweep.py -- per-function compiler-FLAG sweep for the unmatched corpus.

new_campaign_reset (2026-07-20) proved a recorded recipe can simply be WRONG: it was tagged -4r
(register calling) but the real target stack-passes (-4s), and no amount of C reshaping could match
it under -4r. That is not a codegen tie, it is a mislabelled recipe -- and the fix is a flag, not a
source edit. This tool hunts the rest of that class.

For each unmatched function it takes the recipe the compiler actually uses (regdiff.recipe_flags:
recipes.json -> source `Recipe:` header -> default), generates a focused matrix of flag variants
(calling-convention flip, optimisation-bundle swaps, struct-packing), compiles each with the period
Watcom 9.5b toolchain, and runs the SAME reloc/register/slot-aware verdict as regdiff.analyze:
    MATCH > PURE-ALLOC > REGISTER-ROLE > STRUCTURAL
It reports any function whose best non-baseline variant reaches MATCH (an outright new match) or
strictly out-ranks its baseline (a variant removed a real structural divergence -- worth a look).

    docker run --rm --memory=8g -v "$PWD":/work -w /work synd-decomp python3 tools/flag_sweep.py
Writes manifest/flag_sweep.json { name: {baseline:{flags,verdict,score}, best:{...}, all:[...]} }.
Incremental: re-running skips (fn,flags) pairs already recorded unless --fresh.
"""
import json, os, sys, re
import multiprocessing as mp
import regdiff as R

DEFAULT = "-4s -oneatx -zp8 -s -zq"
RANK = {"MATCH": 3, "PURE-ALLOC": 2, "REGISTER-ROLE": 1, "STRUCTURAL": 0, "COMPILE-FAIL": -1}
OUTF = "manifest/flag_sweep.json"


def swap_cc(flags):
    if re.search(r"-4r\b", flags): return re.sub(r"-4r\b", "-4s", flags)
    if re.search(r"-4s\b", flags): return re.sub(r"-4s\b", "-4r", flags)
    return None


def set_opt(flags, opt):
    # replace the -o... optimisation cluster (there is normally exactly one) with `opt`
    if re.search(r"-o\S+", flags):
        return re.sub(r"-o\S+", opt, flags, count=1)
    return flags + " " + opt


def set_pack(flags, zp):
    if re.search(r"-zp\d+\b", flags):
        return re.sub(r"-zp\d+\b", zp, flags)
    return flags + " " + zp


def variants(base):
    """A focused, deduped matrix. Calling-convention flip is the highest-probability lever (proven
    on new_campaign_reset); optimisation-bundle and packing swaps catch grosser mismatches."""
    v = [("baseline", base)]
    cc = swap_cc(base)
    if cc: v.append(("cc-flip", cc))
    for tag, opt in [("opt=-ox", "-ox"), ("opt=-os", "-os"), ("opt=-ot", "-ot"),
                     ("opt=-oaxt", "-oaxt")]:
        v.append((tag, set_opt(base, opt)))
    v.append(("zp4", set_pack(base, "-zp4")))
    v.append(("zp1", set_pack(base, "-zp1")))
    # extension axes: repeated-optimisation / inline modifiers appended to the baseline cluster, and
    # the arch model (target is 386-era; all matched fns use -4, so -3/-5 are a longshot sanity check)
    # note: -oh (repeated-optimisation) is a Watcom 10.x/11.x flag, NOT valid in 9.5 -- both the
    # cluster form (-oneatxh) and a separate -oh token fail to compile, so that axis does not exist here.
    for suff in ("i", "e"):
        v.append((f"opt+{suff}", set_opt(base, "-oneatx" + suff)))
    for a in ("-3", "-5"):
        if re.search(r"-4[rs]\b", base):
            v.append((f"arch{a}", re.sub(r"-4([rs])\b", a + r"\1", base)))
    # dedupe by resulting flag string, keeping the first (most descriptive) tag
    seen, out = set(), []
    for tag, f in v:
        key = " ".join(sorted(f.split()))
        if key not in seen:
            seen.add(key); out.append((tag, f))
    return out


def eval_one(args):
    name, tag, flags = args
    try:
        tb, _ = R.load_target(name)
        c = R.compile_one(name, flags)
        if not c:
            return name, tag, flags, "COMPILE-FAIL", 0.0
        res = R.analyze(tb, c[0], c[1])
        return name, tag, flags, res["verdict"], round(res.get("score", 0.0), 4)
    except Exception as e:
        return name, tag, flags, "COMPILE-FAIL", 0.0


def main():
    man = json.load(open("manifest/functions.json"))["functions"]
    src = set()
    import glob
    for p in glob.glob("src/**/*.c", recursive=True):
        src.add(os.path.basename(p)[:-2])
    unm = [f for f in man if f.get("status") != "matched" and f["name"] in src]
    if "--only" in sys.argv:
        want = set(sys.argv[sys.argv.index("--only") + 1].split(","))
        unm = [f for f in unm if f["name"] in want]
    if "--limit" in sys.argv:
        unm = unm[:int(sys.argv[sys.argv.index("--limit") + 1])]
    w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 4
    prior = {}
    if "--fresh" not in sys.argv and os.path.exists(OUTF):
        prior = json.load(open(OUTF))

    tasks = []
    for f in unm:
        name = f["name"]
        base = R.recipe_flags(name, DEFAULT)
        done = {a["flags"] for a in prior.get(name, {}).get("all", [])}
        for tag, flags in variants(base):
            if flags not in done:
                tasks.append((name, tag, flags))
    print(f"{len(unm)} unmatched fns, {len(tasks)} (fn,variant) compiles to run, {w} workers", flush=True)

    results = {}   # name -> list of (tag, flags, verdict, score)
    with mp.Pool(w) as pool:
        for i, (name, tag, flags, verdict, score) in enumerate(pool.imap_unordered(eval_one, tasks), 1):
            results.setdefault(name, []).append({"tag": tag, "flags": flags, "verdict": verdict, "score": score})
            if i % 25 == 0:
                print(f"  [{i}/{len(tasks)}] ...", flush=True)

    # merge with prior + compute baseline/best per fn
    out = dict(prior)
    for f in unm:
        name = f["name"]
        allr = out.get(name, {}).get("all", []) + results.get(name, [])
        # dedupe by flags, last wins
        byf = {a["flags"]: a for a in allr}
        allr = list(byf.values())
        base = R.recipe_flags(name, DEFAULT)
        baseline = next((a for a in allr if a["flags"] == base), None)
        def key(a): return (RANK.get(a["verdict"], -1), a["score"])
        best = max(allr, key=key) if allr else None
        out[name] = {"baseline": baseline, "best": best, "all": allr}
    json.dump(out, open(OUTF, "w"), indent=0)

    # report
    print(f"\nwrote {OUTF}")
    jackpot, improved = [], []
    for name, d in out.items():
        b, bs = d.get("baseline"), d.get("best")
        if not bs: continue
        if bs["verdict"] == "MATCH" and (not b or b["verdict"] != "MATCH"):
            jackpot.append((name, bs))
        elif b and RANK.get(bs["verdict"], -1) > RANK.get(b["verdict"], -1):
            improved.append((name, b, bs))
    print(f"\n=== JACKPOT: variant reaches MATCH where baseline did not ({len(jackpot)}) ===")
    for name, bs in jackpot:
        print(f"  {name:<28} {bs['tag']:<12} {bs['flags']}")
    print(f"\n=== IMPROVED verdict rank (baseline -> best) ({len(improved)}) ===")
    for name, b, bs in sorted(improved, key=lambda x: -RANK.get(x[2]['verdict'], -1)):
        print(f"  {name:<28} {b['verdict']:<13} -> {bs['verdict']:<13} via {bs['tag']:<10} {bs['flags']}")
    if not jackpot and not improved:
        print("  (none -- recipes are already the best of the swept matrix; residues are genuine ties)")


if __name__ == "__main__":
    main()
