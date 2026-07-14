extern short g_ab60[];
extern short g_ad60[];
extern short g_10b28;
extern short g_10b2a;
void FUN_00026ad8(unsigned short mult, unsigned short idx)
{
    g_10b28 += (g_ab60[idx] * mult) >> 8;
    g_10b2a += (g_ad60[idx] * mult) >> 8;
}
