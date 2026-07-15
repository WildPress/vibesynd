/* @ 0x36188 (27B): if (g_sound_enabled) FUN_00039b0f(g_snd_driver);
 * Dead callee-save wall (3rd sibling of 0x36168/0x39188): dead push ebx/pop ebx because
 * 0x39b0f is declared to clobber ebx and this fn must preserve ebx for its caller. */
extern unsigned char g_sound_enabled;
extern int g_snd_driver;
extern void FUN_00039b0f(int);
#pragma aux FUN_00039b0f modify [ebx];
void FUN_00036188(void)
{
    if (g_sound_enabled != 0)
        FUN_00039b0f(g_snd_driver);
}
