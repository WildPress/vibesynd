/* sound_channel_select @ 0x38fe8 - sound/channel state select. */
extern char g_music_enabled;
extern char g_10b4d;
extern signed char g_cur_seq;
extern int g_seq_state[];
extern int g_seq_ctx;
extern void snd_cmd_ab(int, int);
extern void snd_cmd_aa(int, int);

void sound_channel_select(signed char param_1)
{
    signed char cur;
    if (g_music_enabled != 0 && g_10b4d != 0) {
        cur = g_cur_seq;
        if (cur != param_1) {
            if (cur >= 0)
                snd_cmd_ab(g_seq_ctx, g_seq_state[cur]);
            g_cur_seq = param_1;
            snd_cmd_aa(g_seq_ctx, g_seq_state[param_1]);
        }
    } else if (g_cur_seq >= 0) {
        snd_cmd_ab(g_seq_ctx, g_seq_state[g_cur_seq]);
        g_cur_seq = -1;
    }
}
