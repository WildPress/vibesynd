#!/usr/bin/env python3
"""Whole-module experiment: compile 0x377e8 + 0x37818 in ONE translation unit (address
order) and check whether Watcom tail-merges 0x37818's `return 0` into 0x377e8's stub at
0x3780f -- reproducing the target's cross-function backward jump. Verifies the COMBINED
region 0x377e8..0x3783f reloc-masked (the internal cross-fn jump is NOT a reloc, so it
must match exactly)."""
import os, sys, subprocess, tempfile, shutil
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone

FLAGS = "-4s -oneatx -zp8 -s -zq"
START = 0x377e8
END = 0x3783f            # last byte+1 of 0x37818 (its ret at 0x3783e)
NBYTES = END - START     # 0x57 = 87
IMG = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
TGT = IMG[START - 0x10000: START - 0x10000 + NBYTES]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

COMBO = r"""extern unsigned char g_810e[];
extern unsigned char g_11670;
extern unsigned char g_a6ea[];
extern unsigned char g_a686[];

unsigned short FUN_000377e8(unsigned char *p)
{
    unsigned char *node = g_810e + *(unsigned short *)(p + 0x44);
    if (node >= &g_11670)
        return g_a6ea[node[0x19]];
    return 0;
}

unsigned short FUN_00037818(unsigned char *p)
{
    unsigned char *node = g_810e + *(unsigned short *)(p + 0x44);
    if (node >= g_810e + 0x9562)
        return g_a686[node[0x19]];
    return 0;
}
"""

def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for k in range(off, min(off + size, len(b))):
            b[k] = 0
    return bytes(b)

def dis(code, base):
    return "\n".join(f"  {i.address:06x}: {i.bytes.hex():14s} {i.mnemonic} {i.op_str}"
                     for i in md.disasm(code, base))

def main():
    name = "combo_377e8"
    src = f"src/lib/runtime/{name}.c"
    open(src, "w", newline="\n").write(COMBO)
    try:
        r = subprocess.run(["bash", "tools/wcc_95.sh", name, FLAGS], capture_output=True, text=True)
        if r.returncode != 0:
            print("COMPILE-FAIL:", (r.stdout + r.stderr)[-300:]); return
        ob, fx = text_bytes_and_fixups(f"build/{name}.obj")
        print(f"combined _TEXT = {len(ob)}B, {len(fx)} fixups; target region = {NBYTES}B")
        print("=== OUR combined _TEXT ==="); print(dis(ob[:NBYTES], START))
        n = min(len(ob), NBYTES)
        om, tm = mask(ob[:n], fx), mask(TGT[:n], [(o, s) for o, s in fx if o < n])
        if om == tm:
            print("\n*** COMBINED REGION MATCHES (reloc-masked) -- tail-merge reproduced! ***")
        else:
            fd = next((i for i in range(n) if om[i] != tm[i]), n)
            print(f"\nNO. first masked diff @ region offset 0x{fd:x} (addr 0x{START+fd:x})")
            print("ours :", ob[max(0,fd-2):fd+6].hex())
            print("tgt  :", TGT[max(0,fd-2):fd+6].hex())
    finally:
        os.remove(src)
        for ext in (".obj",):
            p = f"build/{name}{ext}"
            if os.path.exists(p): os.remove(p)
        print("(cleaned up)")

if __name__ == "__main__":
    main()
