/* FUN_00039ee2 (cdecl/-4s, void). Reads the palette/header tail of a chunk into
 * g_5370, then latches the first three words into g_bdd6/g_bdd8/g_bdda.
 * disasm: fread(g_bdd0, g_5370, (unsigned short)g_bddc - 6); p=g_5370;
 *         g_bdd6=p[0]; g_bdd8=p[1]; g_bdda=p[2];
 *
 * NEAR-MISS (best 68/71, recipe -4s -os, first diff at 0x1f). Structure + all
 * relocs byte-correct: the fread call (ff35 push-mem, no saves) and the three
 * word copies match up to registers. Residual = a caller-saves-all register
 * role: the target walks the copy pointer in EDI (used without any save -- the
 * region is compiled caller-saves-everything) and holds each word in AX, which
 * lets it store via the `66 a3` moffs accumulator form; ours (no callee-saved
 * reg to spare without a save) keeps the pointer in EAX and the words in DX,
 * storing via `66 8915`. Our 9.5b will not clobber EDI without saving it, so the
 * target's frameless EDI-pointer + AX-value pairing is unreachable in isolation.
 * §3 register-role / caller-saves-all wall. */
extern int   g_bdd0;
extern void *g_5370;
extern unsigned short g_bddc;
extern unsigned short g_bdd6, g_bdd8, g_bdda;
extern int FUN_0003a7c4(int, void *, int);

void FUN_00039ee2(void)
{
    unsigned short *p;

    FUN_0003a7c4(g_bdd0, g_5370, (unsigned short)g_bddc - 6);
    p = (unsigned short *)g_5370;
    g_bdd6 = p[0];
    g_bdd8 = p[1];
    g_bdda = p[2];
}
