/* 0x35638 -- message-line setter. Sets g_506c=1/g_5064=0, strcpy(g_bdfc,
 * g_b4c0[i]), clears g_10b3f; if g_5591 set, calls 0x4987e(g_10aa0) and clears
 * it; calls 0x39ca0(0,0,g_b958); then the two screen-buffer copies 0x35588 /
 * 0x35538 (both matched) and re-sets g_506c=1.
 * Byte zeros land in AH/DH (Watcom high-byte picks). Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char *g_b4c0[];
extern int g_506c;
extern int g_5064;
extern void FUN_0003a8d7(char *dst, char *src);
extern char g_bdfc[];
extern volatile unsigned char g_10b3f;
extern volatile unsigned char g_5591;
extern unsigned char g_5591w; /* same address; write alias so the store schedules non-volatile (reloc masked) */
extern int g_10aa0;
extern void FUN_0004987e(int a);
extern void FUN_00039ca0(int a, int b, char *c);
extern void bulk_dword_copy_b(void);
extern void bulk_dword_copy(void);
extern char g_b958[];

void FUN_00035638(int i)
{
    g_506c = 1;
    g_5064 = 0;
    FUN_0003a8d7(g_bdfc, g_b4c0[i]);
    g_10b3f = 0;
    if (g_5591 != 0) {
        FUN_0004987e(g_10aa0);
        g_5591w = 0;
    }
    FUN_00039ca0(0, 0, g_b958);
    bulk_dword_copy_b();
    bulk_dword_copy();
    g_506c = 1;
}
