/* 0x279f8 -- scan the far-pointer slot table at 0x10644 (6-byte far ptrs,
 * lgs stride *6). For each slot i < g_num_players: if p[2] and p[0x31]!=0xff, and
 * the slot isn't already marked in g_df30 and isn't self (g_cur_player), report
 * p[0x31] if nonzero via 0x289a8(str 0x376c, 0x10a, c), mark 1 and count;
 * dead slots mark 0. Single shared store g_df30[i]=v (if/else, success path
 * falls through; continue guards skip the store). i unsigned short.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char __far *g_10644[];
extern unsigned char g_df30[];
extern short g_cur_player;
extern short g_num_players;
extern char g_376c[];
extern void FUN_000289a8(char *s, int b, int c);

int FUN_000279f8(void)
{
    int n = 0;
    unsigned short i;

    for (i = 0; i < g_num_players; i++) {
        char __far *p = g_10644[i];

        if (p[2] != 0 && p[0x31] != 0xff) {
            if (g_df30[i] != 0)
                continue;
            if (i == g_cur_player)
                continue;
            if (p[0x31] != 0)
                FUN_000289a8(g_376c, 0x10a, (unsigned char)p[0x31]);
            g_df30[i] = 1;
            n++;
        } else {
            g_df30[i] = 0;
        }
    }
    return n;
}
