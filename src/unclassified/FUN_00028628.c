/* MATCHED (reloc-aware, 135/135) @ 0x28628 -- recipe -4s -oneatx -zp8 -s -zq.
 * The old 132/135 uchar-widen-form park (xor-form vs and-form on the report
 * arg) fell to the cont.21 inline-vs-named identity insight: the and-form
 * (mov al,dl / and eax,0xff) fires only when the pushed value is an ANONYMOUS
 * compiler-owned CSE temp. Fix: no local q, no named st -- every busy-wait /
 * test / arg read written directly as g_5056fp[0x31] (NON-volatile). The lgs
 * stays hoisted, the byte load stays in-loop in DL (far reads are not sunk
 * when the value feeds the loop compare + post-loop uses via one global),
 * post-loop uses CSE onto DL anonymously => and-form widen. Key negatives
 * (all recompiled this session): a named st anywhere (even block-scoped, even
 * with inline post-loop reads that CSE onto its DL) => xor-form; a LOCAL COPY
 * q = g_5056fp with unnamed reads => cmp-mem peeled loop + fresh post-loop
 * load (and-form but wrong loop); full-width temp t=st + (uchar)t cast =>
 * xor+mov+and (all three). The return re-read goes through a third alias
 * (g_5056fp3) so it cannot CSE with the loop value and re-materialises lgs.
 *
   0x28628 -- DOS/DPMI transfer-buffer request, opcode 0x35. The buffer far ptr
 * lives at 0x5056 (offset dword) / 0x505a (selector word); loaded split (mov
 * bx/mov eax) because GS gets re-pointed mid-sequence, selector shadowed in BX.
 * Writes opcode 0x35 to buf[0], copies a word from sel:(off+0x40) into buf+6,
 * submits via 0x27d88(off, sel) returning short (-1 = fail -> -0x63). Then
 * busy-waits on the status byte buf[0x31] until != 0xff (far reads are not
 * hoisted out of the loop; the post-loop q[0x31] uses CSE onto the loop's DL,
 * giving the mov al,dl / and eax,0xff widen); reports unexpected status via
 * 0x289a8(str, 0x273, st) and returns -status as short. g_5056fp/g_5056fp2 =
 * far-ptr aliases of the same 6 bytes (fixups masked); the selector arg reads
 * the word at +4 of the same global so it CSEs onto BX.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char __far *g_5056fp;   /* buffer far ptr (one 6-byte global) */
extern unsigned char __far *g_5056fp2;  /* alias of the same */
extern unsigned char __far *g_5056fp3;  /* alias for the return re-read */
extern char g_376c[];
extern short submit_ncb(unsigned int off, unsigned short sel);
extern void report_net_status(char *s, int b, int c);

short FUN_00028628(unsigned int off, unsigned short sel)
{
    unsigned char __far *p = g_5056fp2;
    unsigned short w;

    p[0] = 0x35;
    w = *(unsigned short __far *)((sel :> (unsigned char *)off) + 0x40);
    *(unsigned short __far *)(p + 6) = w;
    if (submit_ncb((unsigned)p, *((unsigned short *)&g_5056fp2 + 2)) == -1)
        return -0x63;
    while (g_5056fp[0x31] == 0xff)
        ;
    if (g_5056fp[0x31] != 0 && g_5056fp[0x31] != 0x24)
        report_net_status(g_376c, 0x273, g_5056fp[0x31]);
    return -(short)g_5056fp3[0x31];
}
