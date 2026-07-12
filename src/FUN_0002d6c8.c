/* frameless @ 0x2d6c8: twin of 0x2d738. Set obj[0x54] = FUN_0002d8c8(obj, obj[0x55])
   (speed from a table), then step the projectile along direction param_4 using the
   signed direction tables g_ab60 (x) / g_ad60 (y) scaled by obj[0x54] (>>8) added to
   coords obj[4]/obj[6]. Call FUN_0002d468(new_x, new_y, g_10b2c, obj) and return
   whether its (short) result is 0. */
extern short g_10b2c;
extern short g_ab60[];
extern short g_ad60[];
extern int FUN_0002d8c8(unsigned char *obj, short a);
extern int FUN_0002d468(int x, int y, int z, unsigned char *obj);
unsigned short FUN_0002d6c8(unsigned char *obj, unsigned char param_2)
{
    obj[0x54] = FUN_0002d8c8(obj, obj[0x55]);
    return (short)FUN_0002d468(
        (short)((g_ab60[param_2] * obj[0x54] >> 8) + *(short *)(obj + 4)),
        (short)((g_ad60[param_2] * obj[0x54] >> 8) + *(short *)(obj + 6)),
        g_10b2c,
        obj) == 0;
}
