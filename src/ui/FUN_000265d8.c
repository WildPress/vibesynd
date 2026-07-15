/* frameless @ 0x265d8: stats-panel drawer for record p in the 10-byte table at
   0x539c (dword value at +6, signed byte multiplier at +4, owner byte at +2,
   claim word at +0 — the 0x264a8/0x164c8 family). sprintf(buf, fmt_365c,
   value/1000000*14*mult/10) drawn at x=0x10c, sprintf(buf, fmt_3664, mult) at
   x=0x184, static string 0x366c at x=0x160 (all y=0x15c via the FUN_36698 text
   drawer). If owner==g_cur_player with a live claim (word!=0xff), draws two button
   sprites via FUN_4a6c8 using font-table offsets +0x42/+0x48. */
extern char g_365c[];
extern char g_3664[];
extern char g_366c[];
extern unsigned char *g_11be4;
extern unsigned short g_cur_player;
extern unsigned char g_syndicate_recs[];
extern void FUN_0003a4fa(char *buf, char *fmt, unsigned int val);
extern void FUN_00036698(char *s, unsigned short x, int y, int a4, unsigned short colour,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern void FUN_0004a6c8(int x, int y, unsigned char *spr);

void FUN_000265d8(unsigned char p)
{
    char buf[40];
    unsigned int t;

    FUN_0003a4fa(buf, g_365c,
                 *(unsigned int *)(g_syndicate_recs + p * 10 + 6) / 1000000 * 14
                     * (signed char)g_syndicate_recs[p * 10 + 4] / 10);
    FUN_00036698(buf, 0x10c, 0x15c, 0xf, 0x54, g_11be4, -2, 8, 0, 0);
    FUN_0003a4fa(buf, g_3664, (signed char)g_syndicate_recs[p * 10 + 4]);
    FUN_00036698(buf, 0x184, 0x15c, 0xf, 0x54, g_11be4, -2, 8, 0, 0);
    FUN_00036698(g_366c, 0x160, 0x15c, 0xf, 0x54, g_11be4, -2, 8, 0, 0);
    t = g_syndicate_recs[p * 10 + 2];
    if ((unsigned short)t == g_cur_player
        && *(unsigned short *)(g_syndicate_recs + p * 10) != 0xff) {
        FUN_0004a6c8(0x176, 0x15a, g_11be4 + 0x42);
        FUN_0004a6c8(0x1b4, 0x15a, g_11be4 + 0x48);
    }
}
