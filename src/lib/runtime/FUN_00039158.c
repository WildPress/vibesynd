/* @ 0x39158 -- returns (FUN_00039bd7(g_seq_ctx, g_seq_state[g_cur_seq]) == 1). */
extern signed char g_cur_seq;
extern int g_seq_state[];
extern int g_seq_ctx;
extern int FUN_00039bd7(int a, int b);
unsigned short FUN_00039158(void)
{
    return FUN_00039bd7(g_seq_ctx, g_seq_state[g_cur_seq]) == 1;
}
