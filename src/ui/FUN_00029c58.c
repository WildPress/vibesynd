/* @ 0x29c58: HUD icon/text selector. Based on flags g_10afc/g_52ff/g_10b46 and the
   counter g_e395, choose a sprite pointer + a 16-bit frame number, then draw it via
   FUN_0004cde3(sprite, 0, 0xbe, frame, 0x10). The frame=2/g_10564 block sits right
   after the entry test and shares the final call (goto merge) with the frame=1/g_10584
   block at the tail; the g_10554/g_10574 cases are inline. FUN_0004cde3 is EBP-frame
   stack-calling (all args on the stack) -> -4s. Block order mirrors the target CFG. */
extern unsigned char g_10afc, g_52ff, g_10b46, g_e395;
extern unsigned char g_10554[], g_10564[], g_10574[], g_10584[];
extern void FUN_0004cde3(unsigned char *sprite, unsigned short a2,
                         unsigned short a3, unsigned short frame,
                         unsigned short count);

void FUN_00029c58(void)
{
    if ((g_10afc & 4) == 0) goto L_52ff;
block1:
    FUN_0004cde3(g_10564, 0, 0xbe, 2 - g_e395, 0x10);
    return;
L_52ff:
    if (g_52ff != 0) goto L_584;
    if ((g_10b46 & 0x20) == 0) goto L_cc3;
    if ((g_10b46 & 0x10) == 0) goto block1;
    FUN_0004cde3(g_10554, 0, 0xbe, 0, 0x10);
    return;
L_cc3:
    if ((g_10b46 & 0x10) == 0) goto L_ce1;
    FUN_0004cde3(g_10554, 0, 0xbe, 0, 0x10);
    return;
L_ce1:
    if ((g_10afc & 2) == 0) goto block1;
    FUN_0004cde3(g_10574, 0, 0xbe, 2 - g_e395, 0x10);
    return;
L_584:
    FUN_0004cde3(g_10584, 0, 0xbe, 1 - g_e395, 0x10);
    return;
}
