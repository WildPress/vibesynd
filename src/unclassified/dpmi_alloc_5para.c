/* PARKED near-miss (NOT matched, ours 183B vs target 185B, first diff 0x6f)
   -- improved from the old 177/185 park by typing the selector as __segment.
   That fixed old residue (b): fmemset dst offset is now a fresh `xor edi,edi`
   (31ff), and the frame is back to 0x3c with in/out slots right (NOTE: with
   __segment sel the arrays had to be declared out-then-in to keep in at
   base+0 -- __segment changed Watcom's local ordering vs the ushort version).
   ONE residue left, worth exactly the 2 missing bytes plus knock-on forms:
   sel's home. Target homes sel ONLY in GS: direct `mov gs,[esp+0x28]`
   (8e6c2428), spills gs itself (8c6c2438), and re-reads `mov dx,gs` (8cea)
   twice -- hoisted above `test ecx` in the || test, and again at the fmemset
   site before 31ff. Ours homes the value in EDX: `mov edx,[esp+0x28]` +
   `mov gs,dx` (8b542428 8eea), dword spill 89542438, and dx is simply reused
   at both sites (no 8cea at all). Also p/r zero regs come out swapped
   (store gs:[esi+0x40] / return eax=ebx; target is ebx/esi) -- probably
   downstream of the same home choice. Tried and failed to break the EDX home:
   (__segment)out[3] and *(__segment *)(out+3) both load via edx; swapping
   p/r declaration order is a no-op; (__segment)p casts at the use sites
   catastrophically re-read the word through a far pointer to the stack
   (214B). All-inline `sel :> 0` at every site (the 0x28558 lever) loses GS
   entirely (sel -> EBX GPR, 171B); with plain ushort sel, GS survives only
   ONE construction (old finding, still true). Next idea: find a spelling
   whose initial load has no GPR use at all so 9.5b picks the 8e6c2428 form,
   e.g. volatile-ish read, or force edx pressure across the region.
   OLD residue (a) is subsumed: the hoisted-8cea shape is what the GS-only
   home produces; it is not a separate scheduling quirk.

   dpmi_alloc_5para @ 0x27f08 - DPMI (int 0x31, AX=0x100) allocate a 5-paragraph
   DOS memory block, zero its first 0x42 bytes through the returned selector,
   store the real-mode segment (out.ax) at offset 0x40 of the block, and
   return the far pointer sel:>0 in DX:EAX. On carry, report via
   0x289a8(g_376c, 0x1c7, -2) and return far NULL. Cousin of 0x28728.
   Recipe: -4s -oneatx -zp8 -s -zq */

extern void FUN_0003aaf8(void *dst, int val, int len);   /* memset helper */
extern void int386(int a, void *inr, void *outr);  /* int386 */
extern void FUN_000289a8(char *s, int line, int code);
extern char g_376c[];

extern void __far *_fmemset(void __far *dst, int c, unsigned n);
#pragma intrinsic(_fmemset)

unsigned char __far *dpmi_alloc_5para(void)
{
    int out[7];
    int in[7];
    __segment sel;
    unsigned char __far *p;
    unsigned char __far *r;

    FUN_0003aaf8(in, 0, 0x1c);
    FUN_0003aaf8(out, 0, 0x1c);
    in[0] = 0x100;
    in[1] = 5;
    int386(0x31, in, out);
    if (out[6]) {
        FUN_000289a8(g_376c, 0x1c7, -2);
        return 0;
    }
    sel = *(__segment *)(out + 3);
    p = sel :> (unsigned char *)0;
    r = p;
    if (out[6] != 0 || sel != 0) {
        _fmemset(sel :> (unsigned char *)0, 0, 0x42);
        *(unsigned short __far *)(p + 0x40) = (unsigned short)out[0];
    }
    return r;
}
