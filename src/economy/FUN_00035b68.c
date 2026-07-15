/* 0x35b68 -- save game. Taxes the player's cash (dword at g_player_recs + p*0x417)
 * by 10% when above 30000 (unsigned mul/div, RMW -=); builds the path with
 * sprintf(buf, fmt 0x3cf0, str 0xb970, n) like the matched reader 0x35c88;
 * opens 0x18828(buf, 0x222); if fd > 0 writes via 0x188c8: the 0x14-byte name,
 * player recs g_player_recs (0x20b8), research g_syndicate_recs (0x1f4), g_7bf4 (0x2724),
 * g_5780 (0x2286), g_5594 (0x1e9), g_roster_index (4), then closes 0x18878.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_cur_player;
extern unsigned char g_player_recs[];
extern void FUN_0003a4fa(char *buf, char *fmt, char *s, int n);
extern char g_3cf0[];
extern char g_b970[];
extern int FUN_00018828(char *path, int mode);
extern void FUN_000188c8(int fd, void *src, int n);
extern void FUN_00018878(int fd);
extern char g_syndicate_recs[];
extern char g_7bf4[];
extern char g_5780[];
extern char g_5594[];
extern char g_roster_index[];

void FUN_00035b68(char *name, int n)
{
    char buf[100];
    int fd;

    if (*(unsigned int *)(g_player_recs + g_cur_player * 0x417) > 0x7530)
        *(unsigned int *)(g_player_recs + g_cur_player * 0x417) -=
            *(unsigned int *)(g_player_recs + g_cur_player * 0x417) * 10 / 100;
    FUN_0003a4fa(buf, g_3cf0, g_b970, n);
    fd = FUN_00018828(buf, 0x222);
    if (fd > 0) {
        FUN_000188c8(fd, name, 0x14);
        FUN_000188c8(fd, g_player_recs, 0x20b8);
        FUN_000188c8(fd, g_syndicate_recs, 0x1f4);
        FUN_000188c8(fd, g_7bf4, 0x2724);
        FUN_000188c8(fd, g_5780, 0x2286);
        FUN_000188c8(fd, g_5594, 0x1e9);
        FUN_000188c8(fd, g_roster_index, 4);
        FUN_00018878(fd);
    }
}
