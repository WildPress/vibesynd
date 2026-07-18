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
extern unsigned char *g_text_pal;
extern unsigned short g_cur_player;
extern unsigned char g_syndicate_recs[];
extern void sprintf(char *buf, char *fmt, unsigned int val);
extern void draw_ui_text(char *s, unsigned short x, int y, int a4, unsigned short colour,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern void draw_sprite_rle_buf(int x, int y, unsigned char *spr);

void stats_panel_draw(unsigned char p)
{
    char buf[40];
    unsigned int t;

    sprintf(buf, g_365c,
                 *(unsigned int *)(g_syndicate_recs + p * 10 + 6) / 1000000 * 14
                     * (signed char)g_syndicate_recs[p * 10 + 4] / 10);
    draw_ui_text(buf, 0x10c, 0x15c, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
    sprintf(buf, g_3664, (signed char)g_syndicate_recs[p * 10 + 4]);
    draw_ui_text(buf, 0x184, 0x15c, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
    draw_ui_text(g_366c, 0x160, 0x15c, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
    t = g_syndicate_recs[p * 10 + 2];
    if ((unsigned short)t == g_cur_player
        && *(unsigned short *)(g_syndicate_recs + p * 10) != 0xff) {
        draw_sprite_rle_buf(0x176, 0x15a, g_text_pal + 0x42);
        draw_sprite_rle_buf(0x1b4, 0x15a, g_text_pal + 0x48);
    }
}
