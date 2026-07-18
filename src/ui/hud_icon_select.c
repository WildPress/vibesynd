/* @ 0x29c58: HUD icon/text selector. Based on flags g_in_mission/g_52ff/g_blink_tick and the
   counter g_e395, choose a sprite pointer + a 16-bit frame number, then draw it via
   draw_text_fixed(sprite, 0, 0xbe, frame, 0x10). The frame=2/g_10564 block sits right
   after the entry test and shares the final call (goto merge) with the frame=1/g_10584
   block at the tail; the g_10554/g_10574 cases are inline. draw_text_fixed is EBP-frame
   stack-calling (all args on the stack) -> -4s. Block order mirrors the target CFG. */
extern unsigned char g_in_mission, g_52ff, g_blink_tick, g_e395;
extern unsigned char g_10554[], g_10564[], g_10574[], g_10584[];
extern void draw_text_fixed(unsigned char *sprite, unsigned short a2,
                         unsigned short a3, unsigned short frame,
                         unsigned short count);

void hud_icon_select(void)
{
    if ((g_in_mission & 4) == 0) goto L_52ff;
block1:
    draw_text_fixed(g_10564, 0, 0xbe, 2 - g_e395, 0x10);
    return;
L_52ff:
    if (g_52ff != 0) goto L_584;
    if ((g_blink_tick & 0x20) == 0) goto L_cc3;
    if ((g_blink_tick & 0x10) == 0) goto block1;
    draw_text_fixed(g_10554, 0, 0xbe, 0, 0x10);
    return;
L_cc3:
    if ((g_blink_tick & 0x10) == 0) goto L_ce1;
    draw_text_fixed(g_10554, 0, 0xbe, 0, 0x10);
    return;
L_ce1:
    if ((g_in_mission & 2) == 0) goto block1;
    draw_text_fixed(g_10574, 0, 0xbe, 2 - g_e395, 0x10);
    return;
L_584:
    draw_text_fixed(g_10584, 0, 0xbe, 1 - g_e395, 0x10);
    return;
}
