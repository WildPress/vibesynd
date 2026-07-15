extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_10b28;
extern short g_10b2a;
extern int FUN_0004d221(int, int);

int FUN_00026bc8(unsigned short param_1, int param_2, int param_3)
{
    int r;
    unsigned idx;
    short s2 = (short)param_2;
    short s3 = (short)param_3;

    if ((unsigned)(param_1 * param_1)
        <= (unsigned)(s2 * s2 + s3 * s3)) {
        r = FUN_0004d221(s2, s3);
        idx = (unsigned short)r;
        g_10b28 += (g_dir_dx[idx] * param_1) >> 8;
        g_10b2a += (g_dir_dy[idx] * param_1) >> 8;
        return r;
    }
    g_10b28 += param_2;
    g_10b2a += param_3;
    return -1;
}
