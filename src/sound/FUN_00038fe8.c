/* FUN_00038fe8 @ 0x38fe8 - sound/channel state select. */
extern char g_music_enabled;
extern char g_10b4d;
extern signed char g_bbec;
extern int g_11e0c[];
extern int g_11e2c;
extern void FUN_00039bc3(int, int);
extern void FUN_00039bb9(int, int);

void FUN_00038fe8(signed char param_1)
{
    signed char cur;
    if (g_music_enabled != 0 && g_10b4d != 0) {
        cur = g_bbec;
        if (cur != param_1) {
            if (cur >= 0)
                FUN_00039bc3(g_11e2c, g_11e0c[cur]);
            g_bbec = param_1;
            FUN_00039bb9(g_11e2c, g_11e0c[param_1]);
        }
    } else if (g_bbec >= 0) {
        FUN_00039bc3(g_11e2c, g_11e0c[g_bbec]);
        g_bbec = -1;
    }
}
