/* FUN_00039088 (stack/cdecl, -4s). Globals-only guard + dispatch.
 * disasm: if (g_music_enabled && g_10b4d && (signed char)g_cur_seq >= 0)
 *             FUN_00039bc3(g_seq_ctx, g_seq_state[g_cur_seq]); g_cur_seq = -1;
 */
extern unsigned char g_music_enabled;
extern unsigned char g_10b4d;
extern signed char   g_cur_seq;
extern int           g_seq_state[];
extern int           g_seq_ctx;
extern void          FUN_00039bc3(int a, int b);

void FUN_00039088(void)
{
    if (g_music_enabled != 0 && g_10b4d != 0 && g_cur_seq >= 0) {
        FUN_00039bc3(g_seq_ctx, g_seq_state[g_cur_seq]);
        g_cur_seq = -1;
    }
}
