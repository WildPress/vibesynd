/* C runtime: int386 @ 0x3adb2 (CLIB3S, Watcom 9.5). Standard int386.c:
   read the segment registers into a local SREGS, then delegate to __int386x.
   Plain C -- non-leaf, bare `push ebp; mov ebp,esp` frame (no saved regs),
   so matches with the framed recipe -3s -of. Callees are masked call relocs. */
struct SREGS { unsigned short es, cs, ss, ds, fs, gs; };

extern void segread(struct SREGS *sregs);                              /* segread  */
extern int  int386x(int inter_no, const void *in, void *out,          /* __int386x  */
                         struct SREGS *sregs);

int int386(int inter_no, const void *in_regs, void *out_regs)
{
    struct SREGS sregs;

    segread(&sregs);
    return int386x(inter_no, in_regs, out_regs, &sregs);
}
