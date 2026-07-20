#!/usr/bin/env python3
"""ot_audit.py -- is the game's opt bundle -oneatx (time on) or -oneax (time off)? Four functions
match only with -ot OFF. Test the whole corpus: for each fn, compile under its recorded arch with
opt=-oneatx and opt=-oneax (holding -zp8 -s -zq), and see which the original agrees with.
MATCHED fns: does -oneax keep them (regression)? UNMATCHED: does -oneax newly match (new win)?"""
import json, re, sys, multiprocessing as mp
import regdiff as R, match_reloc as M

def match_under(name, flags):
    tb,_=R.load_target(name)
    c=R.compile_one(name, flags)
    if not c: return None
    ob,fx=c
    tm,om=R.mask(tb,fx),R.mask(ob,fx)
    if len(tm)==len(om) and tm==om: return True
    m,tl,pad,nd=M.jumptable_aware_match(tb,ob,fx)
    return bool(m)

def base_arch(name):
    r=R.recipe_flags(name,"-4s -oneatx -zp8 -s -zq")
    arch=next((t for t in r.split() if re.match(r"-[345][rs]$",t)),"-4s")
    return arch

def audit(f):
    name=f["name"]; arch=f["_arch"]
    tx=match_under(name, f"{arch} -oneatx -zp8 -s -zq")
    ax=match_under(name, f"{arch} -oneax -zp8 -s -zq")
    return name, tx, ax, f["status"]

def main():
    man=json.load(open("manifest/functions.json"))["functions"]
    rec=json.load(open("manifest/recipes.json"))
    fns=[dict(f,_arch=base_arch(f["name"])) for f in man
         if (f["name"] in rec or f["status"]!="matched")]
    if "--limit" in sys.argv: fns=fns[:int(sys.argv[sys.argv.index("--limit")+1])]
    w=int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 6
    print(f"opt audit: -oneatx vs -oneax over {len(fns)} fns, {w} workers",flush=True)
    res={"both":[], "only_oneatx":[], "only_oneax":[], "neither":[]}
    newwin=[]
    with mp.Pool(w) as pool:
        for i,(name,tx,ax,st) in enumerate(pool.imap_unordered(audit, fns),1):
            b=("both" if tx and ax else "only_oneatx" if tx else "only_oneax" if ax else "neither")
            res[b].append(name)
            if st!="matched" and ax and not tx: newwin.append(name)  # unmatched that -oneax fixes
            if i%40==0: print(f"  [{i}/{len(fns)}]",flush=True)
    print("\n=== OPT AUDIT (-oneatx vs -oneax) ===")
    for b in ("both","only_oneatx","only_oneax","neither"):
        print(f"  {b:12s}: {len(res[b])}")
    print(f"\nonly_oneatx (need -ot ON -- would BREAK if we switch canonical to -oneax): {len(res['only_oneatx'])}")
    for n in res["only_oneatx"][:40]: print("   ",n)
    print(f"\nUNMATCHED fns that -oneax NEWLY matches (potential new wins): {len(newwin)}")
    for n in newwin: print("   ",n)
    json.dump(res, open("manifest/ot_audit.json","w"), indent=0)

if __name__=="__main__": main()
