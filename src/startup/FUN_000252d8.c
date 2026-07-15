/* frameless linear call-seq @ 0x252d8: guard on two flags, then PIT timer setup + d_setvec */
extern unsigned char g_sound_enabled, g_music_enabled;
extern void FUN_0003b22d(int port, int val);
extern void FUN_0003b273(int a, int b, int c);
extern int g_df08;
extern unsigned short g_df0c;
void FUN_000252d8(void)
{
    if (g_sound_enabled != 0)
        return;
    if (g_music_enabled != 0)
        return;
    FUN_0003b22d(0x43, 0x36);
    FUN_0003b22d(0x40, 0);
    FUN_0003b22d(0x40, 0);
    FUN_0003b273(8, g_df08, g_df0c);
}
