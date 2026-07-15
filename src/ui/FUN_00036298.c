/* FUN_00036298 @ 0x36298 - bounding-box hit test then draw dispatch.
 * If point (g_5390,g_5392) is strictly inside box (g_b6c8,g_b6ca)-(g_b6cc,g_b6ce)
 * pick sprite 0x166 else 0x1ef, then draw via FUN_00036208.
 * Box coords compare UNSIGNED (JBE/JAE) => unsigned short. */

extern volatile unsigned short g_5390, g_5392;
extern unsigned short g_b6c8, g_b6ca, g_b6cc, g_b6ce;
extern unsigned char g_language;
extern unsigned char *g_45a0[];
extern void FUN_00036208(unsigned char *tbl, unsigned short x, unsigned short y,
                         unsigned short w, unsigned short id, int a, int b);

void FUN_00036298(void)
{
    FUN_00036208(g_45a0[g_language], g_b6c8, g_b6ca, g_b6cc - g_b6c8,
                 (g_5392 > g_b6ca && g_5392 < g_b6ce &&
                  g_5390 > g_b6c8 && g_5390 < g_b6cc) ? 0x166 : 0x1ef,
                 -2, 0xc);
}
