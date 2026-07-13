/* FUN_00039ee2 (cdecl/-4s, void). Reads the palette/header tail of a chunk into
 * g_5370, then latches the first three words into g_bdd6/g_bdd8/g_bdda.
 * disasm: fread(g_bdd0, g_5370, (unsigned short)g_bddc - 6); p=g_5370;
 *         g_bdd6=p[0]; g_bdd8=p[1]; g_bdda=p[2];
 */
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
