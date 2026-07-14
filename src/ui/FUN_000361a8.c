/* FUN_000361a8 @ 0x361a8 - centre a string then draw it. Measures the string
 * width via 0x36648 (table g_11be4, base 0x413, adj -2), computes the centred x
 * = (0x280 - width)/2, then draws via 0x36698. The two calls share their
 * trailing stack arguments (nested-call arg overlap). Stack-calling (-4s). */
extern unsigned short FUN_00036648(char *s, unsigned char *tbl, unsigned short base,
                                   signed char adj, int a5);
extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern unsigned char *g_11be4;

void FUN_000361a8(char *param_1)
{
    FUN_00036698(param_1,
                 (unsigned short)((0x280 - FUN_00036648(param_1, g_11be4, 0x413, -2, 0xc)) / 2),
                 0x30, 0x10, 0x413, g_11be4, -2, 0xc, 0, 1);
}
