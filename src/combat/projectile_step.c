/* frameless @ 0x2d738: step a projectile one unit along direction param_2 and test it.
   Set obj[0x54] = 0x80 (speed), then compute the new position from the 256-entry signed
   direction tables g_dir_dx (x) / g_dir_dy (y) scaled by obj[0x54] (>>8), added to the
   current coords obj[4]/obj[6]. Call FUN_0002d5b8(new_x, new_y, g_aim_level, obj) and return
   whether its (short) result is 0. */
extern short g_aim_level;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int FUN_0002d5b8(int x, int y, int z, unsigned char *obj);
int projectile_step(unsigned char *obj, unsigned char param_2)
{
    obj[0x54] = 0x80;
    return (short)FUN_0002d5b8(
        (short)(*(short *)(obj + 4) + (g_dir_dx[param_2] * obj[0x54] >> 8)),
        (short)(*(short *)(obj + 6) + (g_dir_dy[param_2] * obj[0x54] >> 8)),
        g_aim_level,
        obj) == 0;
}
