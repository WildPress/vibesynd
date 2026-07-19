/* 0x35b68 -- save game. Taxes the player's cash (dword at g_player_recs + p*0x417)
 * by 10% when above 30000 (unsigned mul/div, RMW -=); builds the path with
 * sprintf(buf, fmt 0x3cf0, str 0xb970, n) like the matched reader 0x35c88;
 * opens 0x18828(buf, 0x222); if fd > 0 writes via 0x188c8: the 0x14-byte name,
 * player recs g_player_recs (0x20b8), research g_syndicate_recs (0x1f4), g_7bf4 (0x2724),
 * g_list_recs (0x2286), g_5594 (0x1e9), g_roster_index (4), then closes 0x18878.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_cur_player;
extern unsigned char g_player_recs[];
extern void sprintf(char *buf, char *fmt, char *s, int n);
extern char g_3cf0[];
extern char g_b970[];
extern int cond_3call(char *path, int mode);
extern void file_write(int fd, void *src, int n);
extern void file_close(int fd);
extern char g_syndicate_recs[];
extern char g_7bf4[];
extern char g_list_recs[];
extern char g_5594[];
extern char g_roster_index[];

void save_game(char *name, int n)
{
    char buf[100];
    int fd;

    if (*(unsigned int *)(g_player_recs + g_cur_player * 0x417) > 0x7530)
        *(unsigned int *)(g_player_recs + g_cur_player * 0x417) -=
            *(unsigned int *)(g_player_recs + g_cur_player * 0x417) * 10 / 100;
    sprintf(buf, g_3cf0, g_b970, n);
    fd = cond_3call(buf, 0x222);
    if (fd > 0) {
        file_write(fd, name, 0x14);
        file_write(fd, g_player_recs, 0x20b8);
        file_write(fd, g_syndicate_recs, 0x1f4);
        file_write(fd, g_7bf4, 0x2724);
        file_write(fd, g_list_recs, 0x2286);
        file_write(fd, g_5594, 0x1e9);
        file_write(fd, g_roster_index, 4);
        file_close(fd);
    }
}
