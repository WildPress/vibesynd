/* frameless @ 0x27d88: submit a NetBIOS NCB via DPMI. Clears the mailbox status
   byte p[0x31], builds a DPMI real-mode register block (rm.ebx = offset of p,
   flags=0x100, ds/fs = the real-mode segment stored at p+0x40 by the allocator
   0x27f08), zeroes in/out REGS + SREGS, segread(), then __int386x(0x31) with
   in.eax=0x300 (simulate real-mode interrupt), in.ebx=0x5c (int 5Ch, NetBIOS),
   in.edi=&rm. Carry set -> report 0x289a8(g_376c, 0x195, -3), return -1; else 0.
   Callers: 0x284a8 (matched), 0x27fc8, 0x28118.

   IMPROVED to 226/230, EDIT-DIST 70 (was 234/230, dist 74). NEW LEVER that the
   parked notes below said failed: split p INTERNALLY (not the signature) into a
   `__segment sel = (__segment)p` + near `unsigned char *o = (unsigned char *)p`,
   assigned ONCE, and do every far access as `(sel :> o)...`. That homes the
   selector in ESI across the two calls exactly like the target (push esi + the
   2-byte `mov gs,esi` re-arms), killing the parked version's 7-byte
   `mov gs,[slot]` reloads. Note the offset must stay a near ptr and the read
   must keep `__far` (`*(unsigned short __far *)((sel :> o) + 0x40)`), else the
   selector spills to memory / the second gs re-arm is dropped.

   REMAINING GAP (dist 70): the FIRST load. Target reads the two halves as
   separate scalars -- `mov esi,[esp+0x88]` (selector) + `mov ebx,[esp+0x84]`
   (offset) + `mov gs,esi` -- whereas ours fuses them into one
   `lgs eax,[esp+0x84]` (Watcom recognises `sel :> o` == the whole param p and
   loads the far pointer in a single op), then `mov esi,gs` to stash the selector
   for the later re-arm. 4 bytes shorter; the shift cascades through the frame.
   The lgs-vs-split-scalar choice is a codegen tie: forcing the split (read the
   selector via `&p`, or an int offset) either re-fuses byte-identically or
   spills p to memory and loses the ESI cache entirely. Genuine allocator tie;
   fuzzer/cpermute may close it. Recipe: -4s -oneatx -zp8 -s -zq */
extern void memset(void *dst, int val, int len);
extern void segread(void *sregs);
extern void int386x(int inum, void *inr, void *outr, void *sregs);
extern void report_net_status(char *s, int line, int code);
extern char g_376c[];

int submit_ncb(unsigned char __far *p)
{
    char rm[0x32];
    int out[7];
    int in[7];
    int sr[3];
    unsigned short seg;
    __segment sel = (__segment)p;
    unsigned char *o = (unsigned char *)p;

    (sel :> o)[0x31] = 0;
    memset(rm, 0, 0x32);
    seg = *(unsigned short __far *)((sel :> o) + 0x40);
    *(int *)(rm + 0x10) = (int)o;
    *(int *)(rm + 0x20) = 0x100;
    *(unsigned short *)(rm + 0x24) = seg;
    *(unsigned short *)(rm + 0x26) = seg;
    memset(in, 0, 0x1c);
    memset(out, 0, 0x1c);
    memset(sr, 0, 0xc);
    segread(sr);
    in[5] = (int)rm;
    in[0] = 0x300;
    in[1] = 0x5c;
    int386x(0x31, in, out, sr);
    if (out[6] != 0) {
        report_net_status(g_376c, 0x195, -3);
        return -1;
    }
    return 0;
}
