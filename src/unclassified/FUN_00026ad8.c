extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_10b28;
extern short g_10b2a;
void FUN_00026ad8(unsigned short mult, unsigned short idx)
{
    g_10b28 += (g_dir_dx[idx] * mult) >> 8;
    g_10b2a += (g_dir_dy[idx] * mult) >> 8;
}
