/* @ 0x25d58: PARKED near-miss (prologue reg-save-order wall, GAME region).
   Body is byte-faithful (all 6+ calls, constants, globals and the whole
   instruction sequence reproduce the target under -4s -oneatx) BUT the target
   uses a regs-first EBP FRAME: `push ebx;push esi;push edi;push ebp;mov ebp,esp;
   sub esp,0x28`, keeping param1 MEMORY-HOMED at [ebp+0x14] (re-read into bl/dl
   at all 4 use sites, which straddle esp-in-flux nested-call regions). Our
   Watcom 9.5b instead tracks the esp deltas through each cdecl cleanup and
   addresses everything esp-relative -> it OMITS the frame and PROMOTES param1
   into ESI, cascading a param1 EBX<->ESI register-role swap + esp-vs-ebp
   addressing through the whole body. First diff at 0x4 (missing `89 e5`).
   Recipes surveyed: -oneatx/-oneat/-ot/-oi/-ox -> frameless (no 89 e5);
   -of -> frame-FIRST (`55 89 e5 53 56 57`, wrong order); -os -> ENTER-frame,
   drops edi; -od -> EXACT regs-first manual frame `53 56 57 55 89 e5` (first
   diff only at 0x6: sub 0x30/imm32 vs 0x28/imm8) but de-optimizes the body.
   volatile param + inline/library alloca do not induce the optimized frame.
   Same wall class as the 0x3a000 runtime-region prologue reg-save-order wall
   (playbook §3), here proving our 9.5b vs the original build differ on
   frame-omission for esp-in-flux, memory-homed-param functions. TRUE SIZE
   1234B (matches manifest). Semantics below are believed correct.

   mission/agent detail panel drawer. param1 = record index into the parallel
   10-byte g_syndicate_recs table and 19-byte g_b072 table. Centres a title (width via
   0x36648, centred x = 0xb8 + (0x118-w)/2), draws several localized label/value
   rows via the 0x36698 text drawer, then two owner/claim-gated sections.
   Strings come from per-language pointer tables (index g_language*4); the title
   table g_41a4 is 2D (row = g_b072[id]*12). */
extern unsigned char *g_text_pal;
extern unsigned char g_language;
extern unsigned char g_b072[];
extern unsigned char g_syndicate_recs[];
extern unsigned char g_player_recs[];
extern unsigned short g_cur_player;
extern unsigned short g_mouse_x;
extern unsigned short g_mouse_y;
extern char *g_41a4[];
extern char *g_469c[];
extern char *g_46b4[];
extern char *g_46a8[];
extern char *g_46cc[];
extern char *g_46c0[];
extern char *g_45d0[];
extern char *g_46d8[];
extern char *g_46e4[];
extern char *g_46f0[];
extern char *g_46fc[];
extern char *g_4708[];
extern char *g_4714[];
extern char g_365c[];
extern char g_3660[];

extern unsigned short text_width_kern(char *s, unsigned char *tbl, unsigned short base,
                                   signed char adj, int a5);
extern void draw_ui_text(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern void measure_draw_text(char *a, unsigned short b, unsigned short c, unsigned short d,
                         unsigned short e, signed char f, unsigned char g);
extern void sprintf(char *buf, char *fmt, ...);
extern void stats_panel_draw(unsigned char p);
extern char slot_claim_test(unsigned char p);

void draw_agent_detail(unsigned char param1)
{
    char buf[40];
    unsigned short colour;
    short claim;

    /* section 1: centred title */
    draw_ui_text(
        *(char **)((char *)g_41a4 + g_language * 4 + g_b072[param1 * 19] * 12),
        (unsigned short)(0xb8 + (0x118 - text_width_kern(
            *(char **)((char *)g_41a4 + g_language * 4 + g_b072[param1 * 19] * 12),
            g_text_pal, 0x54, -2, 8)) / 2),
        0x13a, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);

    /* section 2 */
    draw_ui_text(*(char **)((char *)g_469c + g_language * 4), 0xc2, 0x14e, 0xf, 0x166,
                 g_text_pal, -2, 8, 0, 0);
    sprintf(buf, g_365c, *(unsigned int *)(g_syndicate_recs + param1 * 10 + 6));
    draw_ui_text(buf, 0x10c, 0x14e, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
    draw_ui_text(*(char **)((char *)g_46b4 + g_language * 4), 0xc2, 0x15c, 0xf, 0x166,
                 g_text_pal, -2, 8, 0, 0);

    /* section 3: owner/claim gate */
    if ((unsigned short)g_syndicate_recs[param1 * 10 + 2] == g_cur_player
        || *(unsigned short *)(g_syndicate_recs + param1 * 10) == 0xff) {
        stats_panel_draw(param1);
        draw_ui_text(*(char **)((char *)g_46a8 + g_language * 4), 0xc2, 0x16a, 0xf, 0x166,
                     g_text_pal, -2, 8, 0, 0);
        if (*(unsigned short *)(g_syndicate_recs + param1 * 10) == 0xff) {
            if (g_mouse_y > 0x15a && g_mouse_y < 0x174 && g_mouse_x > 0x10 && g_mouse_x < 0x92)
                colour = 0x166;
            else
                colour = 0x1ef;
            measure_draw_text(*(char **)((char *)g_45d0 + g_language * 4), 0x10, 0x15a, 0x82,
                         colour, -2, 0xc);
        }
    } else {
        draw_ui_text(*(char **)((char *)g_46cc + g_language * 4), 0x10c, 0x15c, 0xf, 0x54,
                     g_text_pal, -2, 8, 0, 0);
        draw_ui_text(*(char **)((char *)g_46c0 + g_language * 4), 0xc2, 0x16a, 0xf, 0x166,
                     g_text_pal, -2, 8, 0, 0);
        if (slot_claim_test(param1) == 1) {
            if (g_mouse_y > 0x15a && g_mouse_y < 0x174 && g_mouse_x > 0x10 && g_mouse_x < 0x92)
                colour = 0x166;
            else
                colour = 0x1ef;
            measure_draw_text(*(char **)((char *)g_45d0 + g_language * 4), 0x10, 0x15a, 0x82,
                         colour, -2, 0xc);
        }
    }

    /* section 4: owner/claim gate -> status string */
    if ((unsigned short)g_syndicate_recs[param1 * 10 + 2] == g_cur_player
        || *(unsigned short *)(g_syndicate_recs + param1 * 10) == 0xff) {
        claim = *(short *)(g_syndicate_recs + param1 * 10);
        if (claim < 0x2f)
            sprintf(buf, *(char **)((char *)g_46d8 + g_language * 4));
        else if (claim < 0x5f)
            sprintf(buf, *(char **)((char *)g_46e4 + g_language * 4));
        else if (claim < 0xa0)
            sprintf(buf, *(char **)((char *)g_46f0 + g_language * 4));
        else if (claim < 0xd1)
            sprintf(buf, *(char **)((char *)g_46fc + g_language * 4));
        else if (claim < 0xfe)
            sprintf(buf, *(char **)((char *)g_4708 + g_language * 4));
        else
            sprintf(buf, *(char **)((char *)g_4714 + g_language * 4));
    } else {
        sprintf(buf, g_3660,
                     g_player_recs + g_syndicate_recs[param1 * 10 + 2] * 1047 + 0x23);
    }
    draw_ui_text(buf, 0x10c, 0x16a, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
}
