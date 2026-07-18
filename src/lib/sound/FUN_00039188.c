/* @ 0x39188 (25B): if (g_music_enabled) unload_all_drivers(0x3d24);
 * Dead callee-save wall (sibling of 0x36168): the target wraps the guarded call in a
 * dead push ebx/pop ebx because 0x395b6 is declared to clobber ebx (aux pragma = the
 * original TU's header ABI) and this fn must preserve ebx for its own caller. */
extern unsigned char g_music_enabled;
extern void unload_all_drivers(int);
#pragma aux unload_all_drivers modify [ebx];
void FUN_00039188(void)
{
    if (g_music_enabled != 0)
        unload_all_drivers(0x3d24);
}
