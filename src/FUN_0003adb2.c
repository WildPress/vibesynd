/* C runtime: int386 @ 0x3adb2 (CLIB3S, Watcom 9.5). Standard int386.c:
   read the segment registers into a local SREGS, then delegate to int386x.
   Plain C -- non-leaf, bare `push ebp; mov ebp,esp` frame (no saved regs),
   so matches with the framed recipe -3s -of. Callees are masked call relocs. */
struct SREGS { unsigned short es, cs, ss, ds, fs, gs; };

extern void FUN_0003b3b9(struct SREGS *sregs);                              /* segread  */
extern int  FUN_0003b3e6(int inter_no, const void *in, void *out,          /* int386x  */
                         struct SREGS *sregs);

int FUN_0003adb2(int inter_no, const void *in_regs, void *out_regs)
{
    struct SREGS sregs;

    FUN_0003b3b9(&sregs);
    return FUN_0003b3e6(inter_no, in_regs, out_regs, &sregs);
}
