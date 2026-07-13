#!/usr/bin/env bash
set -e
python3 - <<'PY'
import json,sys
sys.path.insert(0,"tools")
from match_reloc import text_bytes_and_fixups, leading_jumptable_entries, MANIFEST, SEG
man=json.load(open(MANIFEST)); base=int(man.get("image_base","0"),16)
f=next(x for x in man["functions"] if x["name"]=="FUN_0002a288")
off=int(f["addr"],16)-base; ts=int(f["size"])
tb=open(SEG,"rb").read()[off:off+ts]
ob,fx=text_bytes_and_fixups("build/FUN_0002a288.obj")
K=leading_jumptable_entries(fx)
cs=len(ob)-ts
print("K=",K,"obj=",len(ob),"ts=",ts,"cs=",cs,"tablelen=",4*K)
open("/tmp/t.bin","wb").write(tb)
open("/tmp/o.bin","wb").write(ob[cs:])
open("/tmp/o2.bin","wb").write(ob[4*K:])
PY
echo "=== TARGET ==="
objdump -D -b binary -m i386 -M intel /tmp/t.bin | sed -n '8,70p'
echo "=== OURS (tail split) ==="
objdump -D -b binary -m i386 -M intel /tmp/o.bin | sed -n '8,70p'
