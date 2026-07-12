#!/bin/bash
for fn in "$@"; do
  bash /work/toolchain/w95b_dl/test95b.sh "$fn" >/dev/null 2>&1
  python3 - "$fn" <<'PY'
import sys,json
sys.path.insert(0,"/work/tools")
from omf import text_bytes_and_fixups
name=sys.argv[1]
man=json.load(open("/work/manifest/functions.json")); base=int(man["image_base"],16)
data=open("/work/inputs/SYNDICAT_MAIN_OBJECT1.linear.bin","rb").read()
f=next(x for x in man["functions"] if x["name"]==name); off=int(f["addr"],16)-base
tb=data[off:off+f["size"]]
def mask(b,fx):
    b=bytearray(b)
    for o,s in fx:
        for j in range(o,min(o+s,len(b))): b[j]=0
    return bytes(b)
try:
    ob,fx=text_bytes_and_fixups(f"/work/build/{name}.obj")
    ok=len(ob)==len(tb) and mask(tb,fx)==mask(ob,fx)
    n=min(len(mask(tb,fx)),len(mask(ob,fx))); mt,mo=mask(tb,fx),mask(ob,fx)
    d=next((i for i in range(n) if mt[i]!=mo[i]), -1)
    print(f"{name}: {'MATCH 9.5b! ' if ok else 'NO'} tgt={len(tb)}B ours={len(ob)}B firstdiff={'-' if ok else hex(d)}")
    if not ok: print(f"  tgt : {tb.hex()[:80]}\n  ours: {ob.hex()[:80]}")
except Exception as e: print(f"{name}: ERR {e}")
PY
done
