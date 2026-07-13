/* PARKED near-miss (NOT matched, 177/185 masked, first diff 0x7c) -- two
   canonicalization-proof diffs, everything else byte-exact incl. the GS-homed
   selector, split zero-offset regs (EBX deref / ESI return), word spill of GS
   to [esp+0x38] and dword reload for the DX:EAX far return:
   (a) target hoists the test's `mov dx,gs` ABOVE `test ecx` (8cea 85c9 7505);
       our 9.5b always sinks the sreg copy into the || second arm (85c9 7507
       8cea). Tried: named temp t=sel, t=(ushort)out[3] CSE spelling, explicit
       two-if goto CFG -- all fold to the same bytes.
   (b) fmemset dst offset: target fresh `xor edi,edi` (31ff); ours copies p's
       zero reg `mov edi,ebx` (89df). A fresh `sel :> 0` arg gives 31ff but
       flips sel out of GS entirely (>=2 explicit :> constructions from sel
       re-home it to EDX/EBX -- see (c) below); q=p copy folds back to 89df.
   KEY LEVER FOUND (playbook-worthy): the selector keeps its GS home only while
   it feeds exactly ONE `:>` construction; every deref/copy must then go
   through that far pointer. The returned copy `r = p` yields the word spill
   `mov [esp+0x38],gs` + xor esi + dword reload tail exactly.

   FUN_00027f08 @ 0x27f08 - DPMI (int 0x31, AX=0x100) allocate a 5-paragraph
   DOS memory block, zero its first 0x42 bytes through the returned selector,
   store the real-mode segment (out.ax) at offset 0x40 of the block, and
   return the far pointer sel:>0 in DX:EAX. On carry, report via
   0x289a8(g_376c, 0x1c7, -2) and return far NULL. Cousin of 0x28728.
   Recipe: -4s -oneatx -zp8 -s -zq */

extern void FUN_0003aaf8(void *dst, int val, int len);   /* memset helper */
extern void FUN_0003adb2(int a, void *inr, void *outr);  /* int386 */
extern void FUN_000289a8(char *s, int line, int code);
extern char g_376c[];

extern void __far *_fmemset(void __far *dst, int c, unsigned n);
#pragma intrinsic(_fmemset)

unsigned char __far *FUN_00027f08(void)
{
    int in[7];
    int out[7];
    unsigned short sel;

    FUN_0003aaf8(in, 0, 0x1c);
    FUN_0003aaf8(out, 0, 0x1c);
    in[0] = 0x100;
    in[1] = 5;
    FUN_0003adb2(0x31, in, out);
    if (out[6]) {
        FUN_000289a8(g_376c, 0x1c7, -2);
        return 0;
    }
    sel = (unsigned short)out[3];
    if (out[6] != 0 || sel != 0) {
        _fmemset(sel :> (unsigned char *)0, 0, 0x42);
        *(unsigned short __far *)((sel :> (unsigned char *)0) + 0x40) = (unsigned short)out[0];
    }
    return sel :> (unsigned char *)0;
}
