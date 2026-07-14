/* frameless @ 0x2d738: step a projectile one unit along direction param_2 and test it.
   Set obj[0x54] = 0x80 (speed), then compute the new position from the 256-entry signed
   direction tables g_ab60 (x) / g_ad60 (y) scaled by obj[0x54] (>>8), added to the
   current coords obj[4]/obj[6]. Call FUN_0002d5b8(new_x, new_y, g_10b2c, obj) and return
   whether its (short) result is 0. */
extern short g_10b2c;
extern short g_ab60[];
extern short g_ad60[];
extern int FUN_0002d5b8(int x, int y, int z, unsigned char *obj);
int FUN_0002d738(unsigned char *obj, unsigned char param_2)
{
    obj[0x54] = 0x80;
    return (short)FUN_0002d5b8(
        (short)(*(short *)(obj + 4) + (g_ab60[param_2] * obj[0x54] >> 8)),
        (short)(*(short *)(obj + 6) + (g_ad60[param_2] * obj[0x54] >> 8)),
        g_10b2c,
        obj) == 0;
}
