/* FUN_00036338 @ 0x36338 - bounding-box hit test then draw dispatch.
 * Direct sibling of FUN_00036298. If point (g_5390,g_5392) is strictly
 * inside box (g_b6d2,g_b6d4)-(g_b6d6,g_b6d8) pick sprite 0x166 else 0x1ef,
 * then draw via FUN_00036208. Box coords compare UNSIGNED (JBE/JAE). */

extern volatile unsigned short g_5390, g_5392;
extern unsigned short g_b6d2, g_b6d4, g_b6d6, g_b6d8;
extern unsigned char g_a50d;
extern unsigned char *g_45ac[];
extern void FUN_00036208(unsigned char *tbl, unsigned short x, unsigned short y,
                         unsigned short w, unsigned short id, int a, int b);

void FUN_00036338(void)
{
    FUN_00036208(g_45ac[g_a50d], g_b6d2, g_b6d4, g_b6d6 - g_b6d2,
                 (g_5392 > g_b6d4 && g_5392 < g_b6d8 &&
                  g_5390 > g_b6d2 && g_5390 < g_b6d6) ? 0x166 : 0x1ef,
                 -2, 0xc);
}
