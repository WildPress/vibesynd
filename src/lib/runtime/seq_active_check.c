/* @ 0x39158 -- returns (sound_cmd_thunks(g_seq_ctx, g_seq_state[g_cur_seq]) == 1). */
extern signed char g_cur_seq;
extern int g_seq_state[];
extern int g_seq_ctx;
extern int sound_cmd_thunks(int a, int b);
unsigned short seq_active_check(void)
{
    return sound_cmd_thunks(g_seq_ctx, g_seq_state[g_cur_seq]) == 1;
}
