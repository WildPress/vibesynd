#!/usr/bin/env python3
"""0x36648: register-role swap in `acc += param_4 + (u16)table[idx].w`. Target puts the
table width in the accumulator and param_4 in the secondary reg; ours reversed. Try
addition operand-order variants to flip the roles (and hopefully the movsx width)."""
import os, sys, subprocess
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone
NAME="FUN_00036648"; SRC="src/ui/FUN_00036648.c"; FLAGS="-4s -oneatx -zp8 -s -zq"
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
IMG=open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin","rb").read()
TGT=IMG[0x36648-0x10000:0x36648-0x10000+79]
H=("struct glyph { char a; char b; char c; char d; unsigned char w; char e; };\n")
SIG="int FUN_00036648(unsigned char *str, struct glyph *table, int param_3, signed char param_4)\n"
def body(expr, extra=""):
    return H+SIG+("{\n    int acc = 0;\n%s    while (*str != 0 && *str != 0x0a) {\n"
        "        int idx = (unsigned short)param_3 + *str - 0x20;\n        %s\n        str++;\n    }\n"
        "    return acc;\n}\n")%(extra, expr)
CANDS={
 "A_base":  body("acc += param_4 + (unsigned short)table[idx].w;"),
 "B_swap":  body("acc += (unsigned short)table[idx].w + param_4;"),
 "C_castint":body("acc += (int)(unsigned short)table[idx].w + param_4;"),
 "D_wtmp":  body("acc += w + param_4;", "    int w;\n").replace("        int w;\n","") if False else
            H+SIG+"{\n    int acc = 0;\n    while (*str != 0 && *str != 0x0a) {\n"
            "        int idx = (unsigned short)param_3 + *str - 0x20;\n"
            "        int w = (unsigned short)table[idx].w;\n        acc += w + param_4;\n        str++;\n    }\n    return acc;\n}\n",
 "E_assoc": body("acc = acc + (unsigned short)table[idx].w + param_4;"),
 "F_p4last":body("acc += (unsigned short)table[idx].w; acc += param_4;"),
}
def cb(b):
    open(SRC,"w",newline="\n").write(b)
    r=subprocess.run(["bash","tools/wcc_95.sh",NAME,FLAGS],capture_output=True,text=True)
    if r.returncode!=0: return None,(r.stdout+r.stderr).strip().splitlines()[-1][:70]
    ob,_=text_bytes_and_fixups(f"build/{NAME}.obj"); return ob,None
def match(b):
    open(SRC,"w",newline="\n").write(b)
    r=subprocess.run(["bash","tools/match95.sh",NAME],capture_output=True,text=True)
    return "RELOC-AWARE match (masked): YES" in r.stdout or "JUMP-TABLE-AWARE match    : YES" in r.stdout
def main():
    bak=open(SRC).read()
    try:
        for k,b in CANDS.items():
            ob,err=cb(b)
            if err: print(f"[{k}] FAIL {err}"); continue
            m=match(b)
            print(f"[{k}] {len(ob)}B {'*** MATCH ***' if m else ''} {ob.hex()}")
            if m: print("WINNER",k); open(SRC,"w",newline="\n").write(b); return
    finally:
        if not any(False for _ in []): pass
        open(SRC,"w",newline="\n").write(bak); print("(restored)")
if __name__=="__main__": main()
