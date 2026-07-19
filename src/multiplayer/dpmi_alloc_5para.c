/* PARKED near-miss (NOT matched, ours 183B vs target 185B, first diff 0x6f,
   EDIT-DIST=25). Improved from the old 29 park by the declaration-order lever:
   declaring `sel` AFTER p and r (locals order p, r, sel) fixes the p/r
   callee-saved role swap. The whole tail now matches target byte-for-byte:
   the offset zeros come out `xor ebx / xor esi` in target order, the store is
   `mov gs:[ebx+0x40],ax` (66 65 89 43 40), and the return is `mov eax,esi`
   (89 f0). Any other position for sel is worse (p s r / r s p = 42, sel-first
   = 29); sel must be declared last.

   ONE residue remains, the whole 0x70-0x8e window and exactly the 2 missing
   bytes: sel's value home. Target homes sel ONLY in GS -- direct
   `mov gs,[esp+0x28]` (8e6c2428), spills GS itself (8c6c2438), and re-derives
   the integer as `mov edx,gs` (8cea) twice (once for the `|| sel!=0` test,
   once for the fmemset ES load). Ours caches the value in EDX:
   `mov edx,[esp+0x28]` + `mov gs,edx` (8b542428 8eea), dword spill 89542438,
   and reuses EDX at both sites (no 8cea at all). This is a register-allocator
   home-choice: our Watcom keeps the selector in a GPR and CSEs every re-read
   spelling back to it. Tried on the 25 baseline, none moved it:
   volatile-read cast *(volatile __segment*)(out+3) (25, CSE'd), a separate
   `unsigned short sv=sel` for the test (25, CSE'd), re-reading out[3] at the
   test site (25, CSE'd), re-reading out[3] at the fmemset site (30, worse),
   `p != 0` far-ptr test (doubled jne, 30), volatile/plain __segment sel
   (47/198B, forces a memory home). Earlier walls still hold: all-inline
   `sel :> 0` loses GS (sel -> EBX GPR, 171B); ushort sel keeps GS for only one
   construction. Genuine allocator home-selection tie: no source spelling seen
   flips the GPR home to GS.

   dpmi_alloc_5para @ 0x27f08 - DPMI (int 0x31, AX=0x100) allocate a 5-paragraph
   DOS memory block, zero its first 0x42 bytes through the returned selector,
   store the real-mode segment (out.ax) at offset 0x40 of the block, and
   return the far pointer sel:>0 in DX:EAX. On carry, report via
   0x289a8(g_376c, 0x1c7, -2) and return far NULL. Cousin of 0x28728.
   Recipe: -4s -oneatx -zp8 -s -zq */

extern void memset(void *dst, int val, int len);   /* memset helper */
extern void int386(int a, void *inr, void *outr);  /* int386 */
extern void report_net_status(char *s, int line, int code);
extern char g_376c[];

extern void __far *_fmemset(void __far *dst, int c, unsigned n);
#pragma intrinsic(_fmemset)

unsigned char __far *dpmi_alloc_5para(void)
{
    int out[7];
    int in[7];
    unsigned char __far *p;
    unsigned char __far *r;
    __segment sel;

    memset(in, 0, 0x1c);
    memset(out, 0, 0x1c);
    in[0] = 0x100;
    in[1] = 5;
    int386(0x31, in, out);
    if (out[6]) {
        report_net_status(g_376c, 0x1c7, -2);
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
