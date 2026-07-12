#!/usr/bin/env python3
"""Emit JSON [{addr, module, cov}] for every function whose bytes match a Watcom
CLIB module at >= threshold coverage. Runs in the container (needs `wlib`).
Reuses libname's mapping logic. Prints JSON to stdout."""
import json, subprocess, re, os, sys
SEG="inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
LIBS=[("95S","toolchain/watcom95/unpacked/CLIB3S.LIB"),
      ("95R","toolchain/watcom95/unpacked/CLIB3R.LIB")]
W=12; THR=float(sys.argv[1]) if len(sys.argv)>1 else 0.55
def load(path):
    data=open(path,"rb").read()
    out=subprocess.run(["wlib",path],capture_output=True,text=True).stdout
    mods=sorted((int(m.group(2),16),m.group(1)) for m in re.finditer(r'^(\S+) Offset=([0-9A-Fa-f]+)H',out,re.M))
    return data,mods
def modat(mods,pos):
    nm=None
    for off,n in mods:
        if off<=pos: nm=n
        else: break
    return nm
libs=[(t,*load(p)) for t,p in LIBS if os.path.exists(p)]
man=json.load(open("manifest/functions.json")); base=int(man["image_base"],16)
data=open(SEG,"rb").read()
out=[]
for f in man["functions"]:
    off=int(f["addr"],16)-base; b=data[off:off+f["size"]]
    if not (b[:3]==b"\x55\x89\xe5" or b[:4]==b"\x53\x55\x89\xe5"): continue
    wins=[b[i:i+W] for i in range(len(b)-W+1)]
    if not wins: continue
    best=(0.0,None)
    for tag,libdata,mods in libs:
        # find best module by locating windows
        hits={}
        for w in wins:
            pos=libdata.find(w)
            if pos>=0:
                nm=modat(mods,pos); hits[nm]=hits.get(nm,0)+1
        if hits:
            nm=max(hits,key=hits.get); cov=hits[nm]/len(wins)
            if cov>best[0]: best=(cov,f"{tag}:{nm}")
    if best[0]>=THR:
        out.append({"addr":f["addr"],"module":best[1],"cov":round(best[0],3),
                    "calls":f["calls"],"status":f["status"]})
print(json.dumps(out,indent=0))
