/* frameless linear call-seq @ 0x252d8: guard on two flags, then PIT timer setup + d_setvec */
extern unsigned char g_sound_enabled, g_music_enabled;
extern void outp(int port, int val);
extern void d_setvec(int a, int b, int c);
extern int g_df08;
extern unsigned short g_df0c;
void timer_setup(void)
{
    if (g_sound_enabled != 0)
        return;
    if (g_music_enabled != 0)
        return;
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);
    d_setvec(8, g_df08, g_df0c);
}
