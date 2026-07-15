/* hittest_draw_b @ 0x36338 - bounding-box hit test then draw dispatch.
 * Direct sibling of hittest_draw. If point (g_mouse_x,g_mouse_y) is strictly
 * inside box (g_b6d2,g_b6d4)-(g_b6d6,g_b6d8) pick sprite 0x166 else 0x1ef,
 * then draw via measure_draw_text. Box coords compare UNSIGNED (JBE/JAE). */

extern volatile unsigned short g_mouse_x, g_mouse_y;
extern unsigned short g_b6d2, g_b6d4, g_b6d6, g_b6d8;
extern unsigned char g_language;
extern unsigned char *g_45ac[];
extern void measure_draw_text(unsigned char *tbl, unsigned short x, unsigned short y,
                         unsigned short w, unsigned short id, int a, int b);

void hittest_draw_b(void)
{
    measure_draw_text(g_45ac[g_language], g_b6d2, g_b6d4, g_b6d6 - g_b6d2,
                 (g_mouse_y > g_b6d4 && g_mouse_y < g_b6d8 &&
                  g_mouse_x > g_b6d2 && g_mouse_x < g_b6d6) ? 0x166 : 0x1ef,
                 -2, 0xc);
}
