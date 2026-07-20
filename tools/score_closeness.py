#!/usr/bin/env python3
"""score_closeness.py -- honest per-function BYTE closeness for the unmatched corpus, for the
treemap/table gradient and for picking real reconstruction targets.

Unlike classify_equiv's structural score (an instruction-alignment diff that DESYNCS on an early
byte difference and then reports a whole function as far-off), this is a reloc-aware masked BYTE
distance via match_reloc.jumptable_aware_match:
    close = 1 - masked_byte_diff / target_size          (1.0 = byte-exact)
It is sub-image-base aware (reads build/obj1_full.bin for addr < 0x10000, which are NOT in the
OBJECT1 linear seg) and jump-table aware (the inline-switch table prefix is stripped, not counted).
It also records delta = ours_len - target_len: a large NEGATIVE delta means our C is MISSING code
(a genuine reconstruction gap), which is a far better target signal than the score.

    docker run --rm --memory=8g -v "$PWD":/work -w /work synd-decomp python3 tools/score_closeness.py
Writes manifest/closeness.json { name: {close, target, ours, delta, jt} }. treemap.py / the byte-match
table read `close` for the red->blue gradient.
"""
import json, os, sys
import multiprocessing as mp
import regdiff
import match_reloc as M

BASE = 0x10000

def target_bytes(f):
    addr = int(f["addr"], 16)
    if addr < BASE and os.path.exists("build/obj1_full.bin"):
        b = open("build/obj1_full.bin", "rb").read(); off = addr - 0xd748
    else:
        b = open(M.SEG, "rb").read(); off = addr - BASE
    return b[off:off + f["size"]]

def score(f):
    name = f["name"]
    tb = target_bytes(f)
    if not tb:
        return name, None
    c = regdiff.compile_one(name, regdiff.recipe_flags(name, "-4s -oneatx -zp8 -s -zq"))
    if not c:
        return name, None
    ob, fx = c
    tm, om = M.mask(tb, fx), M.mask(ob, fx)
    if len(tb) == len(om) and tm == om:
        close, jt = 1.0, False
    else:
        matched, tl, pad, nd = M.jumptable_aware_match(tb, ob, fx)
        if nd is not None:                              # inline jump-table fn: code-only diff
            close, jt = (1.0 if matched else max(0.0, 1.0 - nd / max(len(tb), 1))), tl > 0
        else:                                           # plain fn: masked byte diff + length delta
            n = min(len(tm), len(om))
            d = sum(1 for a, b in zip(tm[:n], om[:n]) if a != b) + abs(len(tb) - len(ob))
            close, jt = max(0.0, 1.0 - d / max(len(tb), 1)), False
    return name, {"close": round(close, 4), "target": len(tb), "ours": len(ob),
                  "delta": len(ob) - len(tb), "jt": jt}

def main():
    man = json.load(open("manifest/functions.json"))["functions"]
    unm = [f for f in man if f.get("status") != "matched"]
    w = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 6
    out = {}
    # --fill: keep the prior closeness.json and only (re)score the entries that are missing or
    # failed (dosemu flakes ~15 under parallel load). Run it with --workers 1 to clear them.
    if "--fill" in sys.argv and os.path.exists("manifest/closeness.json"):
        out = {k: v for k, v in json.load(open("manifest/closeness.json")).items() if v}
        unm = [f for f in unm if not out.get(f["name"])]
        print(f"--fill: {len(unm)} unscored/failed fns to retry, {w} workers", flush=True)
    else:
        print(f"scoring {len(unm)} unmatched fns, {w} workers", flush=True)
    with mp.Pool(w) as pool:
        for i, (name, res) in enumerate(pool.imap_unordered(score, unm), 1):
            if res:
                out[name] = res
            else:
                out.setdefault(name, None)
            tag = (f"{res['close']*100:5.1f}%  d={res['delta']:+5d}  {'JT' if res['jt'] else '  '}"
                   if res else "COMPILE-FAIL")
            print(f"[{i:3}/{len(unm)}] {name:<30} {tag}", flush=True)
    json.dump(out, open("manifest/closeness.json", "w"), indent=0)
    ok = [v for v in out.values() if v]
    gaps = sorted((v for v in ok if v["delta"] <= -12), key=lambda v: v["delta"])
    print(f"\nwrote manifest/closeness.json ({len(out)} fns, {len(out)-len(ok)} compile-fail)")
    print("=== biggest length shortfalls (likely genuine reconstruction gaps) ===")
    inv = {id(v): k for k, v in out.items()}
    for k, v in sorted(out.items(), key=lambda kv: (kv[1]["delta"] if kv[1] else 0)):
        if v and v["delta"] <= -20:
            print(f"  {v['delta']:+5d}B  {v['close']*100:4.0f}%  {k}")


if __name__ == "__main__":
    main()
