/* NEAR-MISS @ 0x28628 -- 132/135 masked; PARKED on a uchar-widen-form wall.
 * Everything matches (split far-ptr load w/ BX selector shadow, :> based-ptr
 * displacement folding, selector arg CSE via word-read of the far-ptr global
 * at +4, FP_OFF push pair, cwde short return, volatile busy-wait on buf[0x31],
 * lgs re-materialisations, -status short return) EXCEPT the report-call arg
 * widen: target = mov al,dl / and eax,0xff (and-form); ours = xor eax,eax /
 * mov al,dl (xor-form), 3 bytes shorter, shifting two je displacements.
 * Tried 12 spellings (casts, K&R promotion [gives mov al,dl + two-step mask],
 * param types char/uchar/int/unsigned, &0xff, second variable, signed char).
 * The and-form fires for compiler CSE temps (cf. 0x279f8's matched inline
 * p[0x31]) but st here is a genuine loop-carried named local; not reachable.
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
extern volatile unsigned char __far *g_5056fp;   /* buffer far ptr (one 6-byte global) */
extern unsigned char __far *g_5056fp2;  /* alias of the same */
extern char g_376c[];
extern short FUN_00027d88(unsigned int off, unsigned short sel);
extern void FUN_000289a8(char *s, int b, int c);

short FUN_00028628(unsigned int off, unsigned short sel)
{
    unsigned char __far *p = g_5056fp2;
    volatile unsigned char __far *q;
    unsigned short w;
    char st;

    p[0] = 0x35;
    w = *(unsigned short __far *)((sel :> (unsigned char *)off) + 0x40);
    *(unsigned short __far *)(p + 6) = w;
    if (FUN_00027d88((unsigned)p, *((unsigned short *)&g_5056fp2 + 2)) == -1)
        return -0x63;
    q = g_5056fp;
    while ((st = q[0x31]) == (char)0xff)
        ;
    if (st != 0 && st != 0x24)
        FUN_000289a8(g_376c, 0x273, (unsigned char)st);
    return -(short)g_5056fp[0x31];
}
