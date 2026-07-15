/* prefix @ 0xe568 -- 16-bit LCG pseudo-random + modulo (previously-undecoded render-path prefix).
 * Advance the seed g_108 = seed*0x24a1 + 0x24df (kept as a 16-bit global), then return seed % range.
 * Values are positive so the original uses signed idiv. Frameless, stack-calling (range at [esp+8]
 * after push ebx). Recipe: -4s -oneatx -zp8 -s -zq */
extern unsigned short g_108;

unsigned FUN_0000e568(unsigned short range)
{
    g_108 = g_108 * 0x24a1 + 0x24df;
    return (int)g_108 % (int)range;
}
