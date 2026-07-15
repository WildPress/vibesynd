/* FUN_00039088 (stack/cdecl, -4s). Globals-only guard + dispatch.
 * disasm: if (g_music_enabled && g_10b4d && (signed char)g_bbec >= 0)
 *             FUN_00039bc3(g_11e2c, g_11e0c[g_bbec]); g_bbec = -1;
 */
extern unsigned char g_music_enabled;
extern unsigned char g_10b4d;
extern signed char   g_bbec;
extern int           g_11e0c[];
extern int           g_11e2c;
extern void          FUN_00039bc3(int a, int b);

void FUN_00039088(void)
{
    if (g_music_enabled != 0 && g_10b4d != 0 && g_bbec >= 0) {
        FUN_00039bc3(g_11e2c, g_11e0c[g_bbec]);
        g_bbec = -1;
    }
}
