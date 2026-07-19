/* @ 0x36188 (27B): if (g_sound_enabled) sound_cmd_thunks_7e_84(g_snd_driver);
 * Dead callee-save wall (3rd sibling of 0x36168/0x39188): dead push ebx/pop ebx because
 * 0x39b0f is declared to clobber ebx and this fn must preserve ebx for its caller. */
extern unsigned char g_sound_enabled;
extern int g_snd_driver;
extern void sound_cmd_thunks_7e_84(int);
#pragma aux sound_cmd_thunks_7e_84 modify [ebx];
void init_sound_driver(void)
{
    if (g_sound_enabled != 0)
        sound_cmd_thunks_7e_84(g_snd_driver);
}
