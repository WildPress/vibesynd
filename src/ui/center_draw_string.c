/* center_draw_string @ 0x361a8 - centre a string then draw it. Measures the string
 * width via 0x36648 (table g_text_pal, base 0x413, adj -2), computes the centred x
 * = (0x280 - width)/2, then draws via 0x36698. The two calls share their
 * trailing stack arguments (nested-call arg overlap). Stack-calling (-4s). */
extern unsigned short text_width_kern(char *s, unsigned char *tbl, unsigned short base,
                                   signed char adj, int a5);
extern void draw_ui_text(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern unsigned char *g_text_pal;

void center_draw_string(char *param_1)
{
    draw_ui_text(param_1,
                 (unsigned short)((0x280 - text_width_kern(param_1, g_text_pal, 0x413, -2, 0xc)) / 2),
                 0x30, 0x10, 0x413, g_text_pal, -2, 0xc, 0, 1);
}
