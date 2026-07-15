/* @ 0x39158 -- returns (FUN_00039bd7(g_11e2c, g_11e0c[g_bbec]) == 1). */
extern signed char g_bbec;
extern int g_11e0c[];
extern int g_11e2c;
extern int FUN_00039bd7(int a, int b);
unsigned short FUN_00039158(void)
{
    return FUN_00039bd7(g_11e2c, g_11e0c[g_bbec]) == 1;
}
