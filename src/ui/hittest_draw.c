/* hittest_draw @ 0x36298 - bounding-box hit test then draw dispatch.
 * If point (g_mouse_x,g_mouse_y) is strictly inside box (g_b6c8,g_b6ca)-(g_b6cc,g_b6ce)
 * pick sprite 0x166 else 0x1ef, then draw via measure_draw_text.
 * Box coords compare UNSIGNED (JBE/JAE) => unsigned short. */

extern volatile unsigned short g_mouse_x, g_mouse_y;
extern unsigned short g_b6c8, g_b6ca, g_b6cc, g_b6ce;
extern unsigned char g_language;
extern unsigned char *g_45a0[];
extern void measure_draw_text(unsigned char *tbl, unsigned short x, unsigned short y,
                         unsigned short w, unsigned short id, int a, int b);

void hittest_draw(void)
{
    measure_draw_text(g_45a0[g_language], g_b6c8, g_b6ca, g_b6cc - g_b6c8,
                 (g_mouse_y > g_b6ca && g_mouse_y < g_b6ce &&
                  g_mouse_x > g_b6c8 && g_mouse_x < g_b6cc) ? 0x166 : 0x1ef,
                 -2, 0xc);
}
