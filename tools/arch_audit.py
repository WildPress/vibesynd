#!/usr/bin/env python3
"""arch_audit.py -- test the 'one global flag' hypothesis. For each MATCHED function, recompile
under -3s and under -4s (holding the recorded non-arch flags) and record which the target agrees
with (reloc-aware). Buckets: BOTH (arbitrary), ONLY-3s, ONLY-4s, NEITHER (an extra flag like
-d2/-of is doing the work). If the target consistently favours one arch, that is the real build
target and the other-arch-only functions are fragile/false matches or wrong C."""
import json, re, sys, multiprocessing as mp
import regdiff as R, match_reloc as M

def match_under(name, flags):
    tb,_=R.load_target(name)
    c=R.compile_one(name, flags)
    if not c: return None
    ob,fx=c
    tm,om=R.mask(tb,fx),R.mask(ob,fx)
    if len(tm)==len(om) and tm==om: return True
    matched,tl,pad,nd=M.jumptable_aware_match(tb,ob,fx)
    return bool(matched)

def swaparch(flags, arch):
    return re.sub(r"-[345][rs]\b", arch, flags) if re.search(r"-[345][rs]\b", flags) else arch+" "+flags

def audit(f):
    name=f["name"]; rec=f.get("_flags","-4s -oneatx -zp8 -s -zq")
    # strip extra flags down to core: keep opt/zp/s/zq, force arch
    core=[t for t in rec.split() if not re.match(r"-[345][rs]$",t)]
    base=" ".join(core)
    m3=match_under(name, "-3s "+base); m4=match_under(name, "-4s "+base)
    return name, m3, m4, rec

def main():
    man=json.load(open("manifest/functions.json"))["functions"]
    rec=json.load(open("manifest/recipes.json"))
    matched=[dict(f, _flags=rec.get(f["name"],{}).get("flags","-4s -oneatx -zp8 -s -zq"))
             for f in man if f["status"]=="matched" and f["name"] in rec]
    if "--limit" in sys.argv: matched=matched[:int(sys.argv[sys.argv.index("--limit")+1])]
    w=int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 6
    print(f"auditing {len(matched)} matched fns under -3s vs -4s, {w} workers",flush=True)
    buckets={"BOTH":[], "ONLY-3s":[], "ONLY-4s":[], "NEITHER":[]}
    with mp.Pool(w) as pool:
        for i,(name,m3,m4,rc) in enumerate(pool.imap_unordered(audit, matched),1):
            b=("BOTH" if m3 and m4 else "ONLY-3s" if m3 else "ONLY-4s" if m4 else "NEITHER")
            buckets[b].append((name,rc))
            if i%40==0: print(f"  [{i}/{len(matched)}]",flush=True)
    print("\n=== ARCH AUDIT (core flags, arch forced) ===")
    for b in ("BOTH","ONLY-3s","ONLY-4s","NEITHER"):
        print(f"  {b:9s}: {len(buckets[b])}")
    json.dump({b:[n for n,_ in v] for b,v in buckets.items()}, open("manifest/arch_audit.json","w"), indent=0)
    print("\nNEITHER (needs an extra flag beyond arch+core -- strongest wrong-C candidates):")
    for n,rc in buckets["NEITHER"][:40]: print(f"  {n:30} recipe: {rc}")

if __name__=="__main__": main()
