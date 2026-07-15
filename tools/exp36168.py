#!/usr/bin/env python3
"""Experiment: find a C formulation of 0x36168 that reproduces the dead push ebx/pop ebx.
Runs in-container. For each candidate body, write src/unclassified/FUN_00036168.c, compile with
the recipe flags, extract _TEXT bytes and disassemble; report exact-match against target."""
import os, sys, subprocess, shutil
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone

NAME = "FUN_00036168"
SRC  = "src/unclassified/FUN_00036168.c"
FLAGS = "-4s -oneatx -zp8 -s -zq"
TARGET = bytes.fromhex("53803d4a0b010000740a6a00e83d34000083c4045bc3")
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

# Each candidate is a full .c file body.
HEAD = "extern unsigned char g_10b4a;\n"
CANDS = {
 "A_baseline": HEAD + "extern void FUN_0000344e(int);\n"
   "void FUN_00036168(void){ if(g_10b4a!=0) FUN_0000344e(0); }\n",
 "B_local_char": HEAD + "extern void FUN_0000344e(int);\n"
   "void FUN_00036168(void){ unsigned char x=g_10b4a; if(x!=0) FUN_0000344e(0); }\n",
 "C_retval_unused": HEAD + "extern int FUN_0000344e(int);\n"
   "void FUN_00036168(void){ if(g_10b4a!=0) FUN_0000344e(0); }\n",
 "D_ptr": HEAD + "extern void FUN_0000344e(int);\n"
   "void FUN_00036168(void){ unsigned char *p=&g_10b4a; if(*p!=0) FUN_0000344e(0); }\n",
 "E_register": HEAD + "extern void FUN_0000344e(int);\n"
   "void FUN_00036168(void){ register unsigned char x=g_10b4a; if(x) FUN_0000344e(0); }\n",
 # callee declared to MODIFY ebx (destroys it) via pragma aux -- may force a save at call sites
 "F_callee_modifies_ebx": HEAD +
   "extern void FUN_0000344e(int);\n"
   "#pragma aux FUN_0000344e modify [ebx];\n"
   "void FUN_00036168(void){ if(g_10b4a!=0) FUN_0000344e(0); }\n",
 # value held across the call in a callee-saved reg then discarded
 "G_live_across": HEAD + "extern int FUN_0000344e(int);\nextern int g_x;\n"
   "void FUN_00036168(void){ int t=g_x; if(g_10b4a!=0){ FUN_0000344e(0);} g_x=t; }\n",
 # loop counter that lands in ebx but body empty
 "H_dummy_loop": HEAD + "extern void FUN_0000344e(int);\n"
   "void FUN_00036168(void){ if(g_10b4a!=0) FUN_0000344e(0); }\n",
}

def compile_and_bytes(body):
    open(SRC, "w", newline="\n").write(body)
    r = subprocess.run(["bash", "tools/wcc_95.sh", NAME, FLAGS],
                       capture_output=True, text=True)
    if r.returncode != 0:
        return None, "COMPILE-FAIL: " + (r.stdout + r.stderr).strip().splitlines()[-1][:80]
    ob, _ = text_bytes_and_fixups(f"build/{NAME}.obj")
    return ob, None

def disasm(code, base=0x36168):
    out = []
    for ins in md.disasm(code, base):
        out.append(f"  {ins.bytes.hex():14s} {ins.mnemonic} {ins.op_str}")
    return "\n".join(out)

def main():
    backup = open(SRC).read()
    print("TARGET (%dB): %s" % (len(TARGET), TARGET.hex()))
    print(disasm(TARGET)); print("="*60)
    try:
        for k, body in CANDS.items():
            ob, err = compile_and_bytes(body)
            if err:
                print(f"[{k}] {err}"); continue
            has_ebx = ob[:1] == b"\x53"
            exact = ob == TARGET
            tag = "*** EXACT MATCH ***" if exact else ("push-ebx OK" if has_ebx else "no ebx")
            print(f"[{k}] {len(ob)}B {tag}  {ob.hex()}")
            if has_ebx and not exact:
                print(disasm(ob))
    finally:
        open(SRC, "w", newline="\n").write(backup)
        print("(restored original source)")

if __name__ == "__main__":
    main()
