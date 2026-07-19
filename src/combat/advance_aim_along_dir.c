extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_aim_x;
extern short g_aim_y;
void advance_aim_along_dir(unsigned short mult, unsigned short idx)
{
    g_aim_x += (g_dir_dx[idx] * mult) >> 8;
    g_aim_y += (g_dir_dy[idx] * mult) >> 8;
}
