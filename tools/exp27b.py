#!/usr/bin/env python3
"""0x27ed8 round 2: form F loads p2 as word but schedules p1 first. Target schedules p2 first.
Try source nudges + scheduling flags to flip the two independent loads."""
import os, sys, subprocess
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone
NAME="FUN_00027ed8"; SRC="src/unclassified/FUN_00027ed8.c"
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
IMG=open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin","rb").read()
TGT=IMG[0x27ed8-0x10000:0x27ed8-0x10000+34]
H="extern void FUN_000287c8(int a, int b);\n"
# source variants (all should keep word-load + (p1,p2) offsets)
SRCS={
 "F":    H+"void FUN_00027ed8(int p1, unsigned short p2){ if(p1){FUN_000287c8(p1,p2);return;} if(p2)FUN_000287c8(p1,p2); }\n",
 "F2":   H+"void FUN_00027ed8(int p1, unsigned short p2){ unsigned short b=p2; if(p1){FUN_000287c8(p1,b);return;} if(b)FUN_000287c8(p1,b); }\n",
 "F3":   H+"void FUN_00027ed8(int p1, unsigned short p2){ if(p2){ if(p1||p2)FUN_000287c8(p1,p2);} else if(p1)FUN_000287c8(p1,p2); }\n",
 "F4":   H+"void FUN_00027ed8(int p1, unsigned short p2){ int a=p1; if(p2!=0){FUN_000287c8(a,p2);return;} if(a)FUN_000287c8(a,p2); }\n",
}
FLAGSETS=["-4s -oneatx -zp8 -s -zq","-4s -otexan -zp8 -s -zq","-4s -oaxt -zp8 -s -zq",
          "-4s -os -zp8 -s -zq","-4s -oneatxr -zp8 -s -zq","-4s -oi -oneatx -zp8 -s -zq",
          "-4r -oneatx -zp8 -s -zq","-4s -oneatx -zp4 -s -zq"]
def cb(body,flags):
    open(SRC,"w",newline="\n").write(body)
    r=subprocess.run(["bash","tools/wcc_95.sh",NAME,flags],capture_output=True,text=True)
    if r.returncode!=0: return None
    ob,_=text_bytes_and_fixups(f"build/{NAME}.obj"); return ob
def mask(b):  # crude: zero call rel32 (last e8 ....) -- find e8 near end
    return b
def eqmasked(ob):
    # match95 reloc-aware
    r=subprocess.run(["bash","tools/match95.sh",NAME],capture_output=True,text=True)
    return "RELOC-AWARE match (masked): YES" in r.stdout
def main():
    bak=open(SRC).read()
    print("target:",TGT.hex())
    try:
        for sk,body in SRCS.items():
            for flags in FLAGSETS:
                ob=cb(body,flags)
                if ob is None: continue
                open(SRC,"w",newline="\n").write(body)
                ok=eqmasked(ob)
                tag="*** MATCH ***" if ok else ""
                # only print near-length ones
                if ok or len(ob)==34:
                    print(f"[{sk} | {flags}] {len(ob)}B {tag} {ob.hex()}")
                if ok:
                    print("WINNER src=%s flags=%s"%(sk,flags)); return
    finally:
        open(SRC,"w",newline="\n").write(bak); print("(restored)")
if __name__=="__main__": main()
