#!/usr/bin/env python3
"""Experiment: crack 0x27ed8 scheduling/width. Target loads p2 (word) before p1, tests p1 first,
passes (p1, (u16)p2). Find a C form that loads p2 as word BEFORE p1."""
import os, sys, subprocess
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone
NAME="FUN_00027ed8"; SRC="src/unclassified/FUN_00027ed8.c"; FLAGS="-4s -oneatx -zp8 -s -zq"
TARGET=bytes.fromhex(open("/dev/stdin").read().strip()) if False else None
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
IMG=open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin","rb").read()
TGT=IMG[0x27ed8-0x10000:0x27ed8-0x10000+34]

H="extern void FUN_000287c8(int a, int b);\n"
CANDS={
 "A_base":       H+"void FUN_00027ed8(int p1, unsigned short p2){ if(p1!=0||p2!=0) FUN_000287c8(p1,p2); }\n",
 "B_or_swap":    H+"void FUN_00027ed8(int p1, unsigned short p2){ if(p2!=0||p1!=0) FUN_000287c8(p1,p2); }\n",
 "C_bitor":      H+"void FUN_00027ed8(int p1, unsigned short p2){ if((p1|p2)!=0) FUN_000287c8(p1,p2); }\n",
 "D_temp_p2":    H+"void FUN_00027ed8(int p1, unsigned short p2){ unsigned short b=p2; if(p1!=0||b!=0) FUN_000287c8(p1,b); }\n",
 "E_p2_int":     H+"void FUN_00027ed8(int p1, int p2){ if(p1!=0||p2!=0) FUN_000287c8(p1,(unsigned short)p2); }\n",
 "F_nested":     H+"void FUN_00027ed8(int p1, unsigned short p2){ if(p1){FUN_000287c8(p1,p2);return;} if(p2)FUN_000287c8(p1,p2); }\n",
 "G_notboth0":   H+"void FUN_00027ed8(int p1, unsigned short p2){ if(!(p1==0&&p2==0)) FUN_000287c8(p1,p2); }\n",
 "H_short_first":H+"void FUN_00027ed8(unsigned short p2, int p1){ if(p1!=0||p2!=0) FUN_000287c8(p1,p2); }\n",
}
def compile_bytes(body):
    open(SRC,"w",newline="\n").write(body)
    r=subprocess.run(["bash","tools/wcc_95.sh",NAME,FLAGS],capture_output=True,text=True)
    if r.returncode!=0: return None,(r.stdout+r.stderr).strip().splitlines()[-1][:70]
    ob,_=text_bytes_and_fixups(f"build/{NAME}.obj"); return ob,None
def dis(code,base=0x27ed8):
    return "\n".join(f"    {i.bytes.hex():12s} {i.mnemonic} {i.op_str}" for i in md.disasm(code,base))
def firstdiff(a,b):
    for i in range(min(len(a),len(b))):
        if a[i]!=b[i]: return i
    return min(len(a),len(b)) if len(a)!=len(b) else -1
def main():
    bak=open(SRC).read()
    print("TARGET(%dB):"%len(TGT)); print(dis(TGT)); print("="*50)
    try:
        for k,body in CANDS.items():
            ob,err=compile_bytes(body)
            if err: print(f"[{k}] FAIL {err}"); continue
            # reloc-aware compare via match95
            open(SRC,"w",newline="\n").write(body)
            r=subprocess.run(["bash","tools/match95.sh",NAME],capture_output=True,text=True)
            m="MATCH" if ("RELOC-AWARE match (masked): YES" in r.stdout or "JUMP-TABLE-AWARE match    : YES" in r.stdout) else "no"
            fd=firstdiff(ob,TGT)
            print(f"[{k}] {len(ob)}B reloc={m} firstdiff@{fd}  {ob.hex()}")
            if m=="MATCH": print("  *** RELOC MATCH ***"); break
    finally:
        open(SRC,"w",newline="\n").write(bak); print("(restored)")
if __name__=="__main__": main()
