/* FUN_00036208 @ 0x36208 - measure+draw marshalling, cousin of 0x361a8.
 * Measures via 0x36648 (table g_text_pal), computes a centred/offset x, then
 * draws via 0x36698. The two calls share their trailing stack arguments
 * (nested-call arg overlap). Stack-calling (-4s). */
extern unsigned short FUN_00036648(char *s, unsigned char *tbl, unsigned short base,
                                   signed char adj, int a5);
extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, unsigned char a8, int a9,
                         int a10);
extern unsigned char *g_text_pal;

void FUN_00036208(char *a, unsigned short b, unsigned short c, unsigned short d,
                  unsigned short e, signed char f, unsigned char g)
{
    FUN_00036698(a,
                 (unsigned short)(b + (d - FUN_00036648(a, g_text_pal, e, f, g)) / 2),
                 (unsigned short)(c + 8), 0xa, e, g_text_pal, f, g, 0, 0);
}
