/* frameless @ 0x2d6c8: twin of 0x2d738. Set obj[0x54] = anim_speed_select(obj, obj[0x55])
   (speed from a table), then step the projectile along direction param_4 using the
   signed direction tables g_dir_dx (x) / g_dir_dy (y) scaled by obj[0x54] (>>8) added to
   coords obj[4]/obj[6]. Call path_probe_y(new_x, new_y, g_aim_level, obj) and return
   whether its (short) result is 0. */
extern short g_aim_level;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int anim_speed_select(unsigned char *obj, short a);
extern int path_probe_y(int x, int y, int z, unsigned char *obj);
unsigned short aim_step(unsigned char *obj, unsigned char param_2)
{
    obj[0x54] = anim_speed_select(obj, obj[0x55]);
    return (short)path_probe_y(
        (short)((g_dir_dx[param_2] * obj[0x54] >> 8) + *(short *)(obj + 4)),
        (short)((g_dir_dy[param_2] * obj[0x54] >> 8) + *(short *)(obj + 6)),
        g_aim_level,
        obj) == 0;
}
