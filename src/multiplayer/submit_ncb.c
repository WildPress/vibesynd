/* frameless @ 0x27d88: submit a NetBIOS NCB via DPMI. Clears the mailbox status
   byte p[0x31], builds a DPMI real-mode register block (rm.ebx = offset of p,
   flags=0x100, ds/fs = the real-mode segment stored at p+0x40 by the allocator
   0x27f08), zeroes in/out REGS + SREGS, segread(), then int386x(0x31) with
   in.eax=0x300 (simulate real-mode interrupt), in.ebx=0x5c (int 5Ch, NetBIOS),
   in.edi=&rm. Carry set -> report 0x289a8(g_376c, 0x195, -3), return -1; else 0.
   Callers: 0x284a8 (matched), 0x27fc8, 0x28118.

   PARKED at 234/230 (everything beyond the entry aligns modulo the GS forms):
   PARAM-PROMOTION wall, selector flavour. Target homes the far param's selector
   half in ESI (`mov esi,[esp+0x88]` + `mov gs,si` re-arms, push esi); ours
   always re-loads GS from the param slot (`mov gs,[esp+0x8c]`, 7B vs 3B). Tried:
   far-ptr param direct, named far local copy, (off, ushort sel) split params,
   __segment param, ushort local copy � all copy-propagate back to slot loads.
   Same profitability rule as 0x35d08/0x2e808, opposite direction. */
extern void FUN_0003aaf8(void *dst, int val, int len);
extern void segread(void *sregs);
extern void FUN_0003b3e6(int inum, void *inr, void *outr, void *sregs);
extern void FUN_000289a8(char *s, int line, int code);
extern char g_376c[];

int submit_ncb(unsigned char __far *p)
{
    char rm[0x32];
    int out[7];
    int in[7];
    int sr[3];
    unsigned short seg;
    unsigned char __far *q;

    q = p;
    q[0x31] = 0;
    FUN_0003aaf8(rm, 0, 0x32);
    seg = *(unsigned short __far *)(q + 0x40);
    *(int *)(rm + 0x10) = (int)q;
    *(int *)(rm + 0x20) = 0x100;
    *(unsigned short *)(rm + 0x24) = seg;
    *(unsigned short *)(rm + 0x26) = seg;
    FUN_0003aaf8(in, 0, 0x1c);
    FUN_0003aaf8(out, 0, 0x1c);
    FUN_0003aaf8(sr, 0, 0xc);
    segread(sr);
    in[5] = (int)rm;
    in[0] = 0x300;
    in[1] = 0x5c;
    FUN_0003b3e6(0x31, in, out, sr);
    if (out[6] != 0) {
        FUN_000289a8(g_376c, 0x195, -3);
        return -1;
    }
    return 0;
}
