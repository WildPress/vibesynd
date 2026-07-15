/* FUN_0002c578 @ 0x2c578 -- AGENT STATUS-PANEL / HUD RENDERER (per-player squad).
 *
 * TRUE SIZE = 2387 bytes (0x953), body 0x2c578 - 0x2ceca (RET at 0x2ceca).
 *   The manifest records 2080 (0x820, ends 0x2cd97) -- UNDERCOUNTED. Ghidra's DB
 *   stops the function at 0x2cd97, mid-way through the second loop, because the
 *   7-byte `MOVSX EDX,[EAX+0xe552]` at 0x2cd91 desyncs its linear sweep. The code
 *   past 0x2cd97 is a genuine continuation: a second walk that shares the single
 *   `ADD ESP,8; POP EBP/EDI/ESI/EBX; RET` epilogue at 0x2cec3 which mirrors the
 *   `PUSH EBX/ESI/EDI/EBP; SUB ESP,8` prologue. => manifest size must be 2387.
 *
 * WHAT IT DOES
 *   1. Clears the panel region (FUN_00018ab8(0,0,0x80,0x190) stores the clip
 *      rectangle globals g_5380..g_538c = 0,0,128,400).
 *   2. Advances a blink tick (g_blink_tick++) and rebuilds an 8-entry blink table:
 *      g_e394[k] = (g_blink_tick / (k+1)) & 1   -- eight phase-shifted blink flags.
 *   3. For each of the player's 4 agents (i=0..3; entity =
 *      g_pool_a + (g_e551[player*0x417] + i)*0x5c), if the agent is active
 *      (entity[0x1d] & 4):
 *        - if entity[0xb] & 1 (firing / special): run a 0->1->2 muzzle-flash
 *          state machine in the per-agent cache byte +10, raise the panel flag
 *          g_hud_panel[i*0x12 + 8], and draw only the inventory icon (skip the bars).
 *        - else draw three horizontal IPA-style stat bars (Adrenaline/Perception/
 *          Intelligence -- entity bytes 0x47..0x49, 0x4f..0x51, 0x4b..0x4d each
 *          scaled *55/255 into a 55px bar) with a change animation, then a
 *          vertical health bar (entity[0x14] word), then the icon.
 *        Each bar is redrawn only when its cached value(s) change (dirty tracking
 *        via the 11-byte per-agent cache at g_df48 + i*0xb).
 *   4. A second pass walks a chain of up to 8 auxiliary objects hung off the
 *      squad-reference entity's +0x3a link (chained via +0x1c) and draws a small
 *      4px progress bar for each enabled type (g_frame_enable[frame]!=0), value =
 *      clamp((hp+1)*23/(g_a73a[frame]+1), 23), cached in g_df76[i].
 *
 * DRAW PRIMITIVES (all in the hand-asm graphics region, args below in C order):
 *   FUN_0003feb3(x, y, w, h, color)   -- filled rectangle / bar segment.
 *   FUN_0003f575(y0, y1, x, color)    -- clipped vertical line (bar end cap).
 *   FUN_0001b908(entity, idx, x, y)   -- draw the agent's weapon/inventory icon.
 *   FUN_00018ab8(a, b, c, d)          -- store the 4 clip-rect globals.
 *
 * The three IPA channels are byte-identical in the binary apart from their entity
 * field, cache offset, y position and colour pair; they are captured here by the
 * IPA() macro (expands inline, exactly as the compiler emitted them -- no call).
 *
 * STATUS: PARKED. Register-role / accumulator walls (the repeated `*55 IDIV 255`
 * chains and the two-segment redraw's stack-shuffled colour push) are not
 * source-reachable, and the wrong manifest size (2080 vs real 2387) makes
 * match_reloc length-mismatch regardless. Recipe: -4s -oneatx -zp8 -s -zq.
 */

extern unsigned char  g_blink_tick;      /* blink tick counter */
extern unsigned char  g_e394[];     /* 8 phase-shifted blink flags */
extern short          g_cur_player;      /* current player index */
extern unsigned char  g_e551[];     /* player record +0xb5: squad first-agent slot */
extern signed char    g_e552[];     /* player record +0xb6: reference-entity delta */
extern unsigned char  g_pool_a[];     /* pool A: entity record k at +k*0x5c */
extern unsigned char  g_entity_pool[];     /* pool A base-2: node = g_entity_pool + id */
extern unsigned char  g_hud_panel[];     /* agent HUD panel table, stride 0x12: x@+0,y@+2,flag@+8 */
extern unsigned char  g_df48[];     /* per-agent 11-byte HUD value cache */
extern unsigned char  g_df76[];     /* aux-bar value cache (8 bytes) */
extern unsigned char  g_auxbar_panel[];     /* aux-bar panel table, stride 0x12: x@+0,y@+2 */
extern unsigned short g_a73a[];     /* per-frame max/quantity table */
extern unsigned char  g_frame_enable[];     /* per-frame enable flags */

extern void FUN_00018ab8(int a, int b, int c, int d);
extern void FUN_0003feb3(int x, int y, int w, int h, int color);
extern void FUN_0003f575(int y0, int y1, int x, int color);
extern void FUN_0001b908(int p, unsigned short idx, int x, int y);

#define HUD_X(i) (*(unsigned short *)(g_hud_panel + (i) * 0x12))
#define HUD_Y(i) (*(unsigned short *)(g_hud_panel + (i) * 0x12 + 2))

/* One horizontal IPA stat bar with change animation.
 *   FMID  = middle entity field (bar is entity[FMID-1], entity[FMID], entity[FMID+1])
 *   CB    = offset of this channel's 3-byte cache within the agent's 11-byte record
 *   YOFF  = bar y offset within the panel
 *   C1/C2 = the two segment colours (path1 draws C1 then C2, path2 draws C2 then C1)
 */
#define IPA(FMID, CB, YOFF, C1, C2)                                                    \
    do {                                                                              \
        mid = (int)(unsigned char)p[(FMID)] * 55 / 255;                               \
        lo  = (int)(unsigned char)p[(FMID) - 1] * 55 / 255;                           \
        hi  = (int)(unsigned char)p[(FMID) + 1] * 55 / 255;                           \
        if (g_df48[i * 0xb + (CB)] != lo || g_df48[i * 0xb + (CB) + 1] != mid ||      \
            g_df48[i * 0xb + (CB) + 2] != hi) {                                       \
            FUN_0003feb3(HUD_X(i) + 4, HUD_Y(i) + (YOFF), 0x37, 0xa, 0);              \
            if ((mid < hi && hi > lo) || (mid > hi && hi < lo)) {                     \
                FUN_0003feb3(HUD_X(i) + 4 + mid, HUD_Y(i) + (YOFF), lo - mid, 0xa, (C1)); \
                FUN_0003feb3(HUD_X(i) + 4 + mid, HUD_Y(i) + (YOFF), lo - mid, 0xa, (C2)); \
            } else {                                                                  \
                FUN_0003feb3(HUD_X(i) + 4 + mid, HUD_Y(i) + (YOFF), lo - mid, 0xa, (C2)); \
                FUN_0003feb3(HUD_X(i) + 4 + mid, HUD_Y(i) + (YOFF), hi - mid, 0xa, (C1)); \
            }                                                                         \
            FUN_0003f575(HUD_Y(i) + (YOFF), HUD_Y(i) + (YOFF) + 9, HUD_X(i) + 4, 0xc);\
            g_df48[i * 0xb + (CB)] = lo;                                              \
            g_df48[i * 0xb + (CB) + 1] = mid;                                         \
            g_df48[i * 0xb + (CB) + 2] = hi;                                          \
        }                                                                             \
    } while (0)

void FUN_0002c578(void)
{
    int i;
    int lo, mid, hi;
    unsigned char *p;
    unsigned short id;

    FUN_00018ab8(0, 0, 0x80, 0x190);

    g_blink_tick++;
    for (i = 0; i < 8; i++)
        g_e394[i] = (unsigned char)(((int)(unsigned char)g_blink_tick / (i + 1)) & 1);

    for (i = 0; i < 4; i++) {
        p = g_pool_a + ((int)(unsigned char)g_e551[(int)g_cur_player * 0x417] + i) * 0x5c;

        if ((p[0x1d] & 4) == 0)
            continue;

        if (p[0xb] & 1) {
            /* firing / special: muzzle-flash state machine, icon only */
            unsigned char st = g_df48[i * 0xb + 10];
            if (st == 0) {
                g_hud_panel[i * 0x12 + 8] = 1;
                g_df48[i * 0xb + 10] = 1;
            } else if (st == 1) {
                g_df48[i * 0xb + 10] = 2;
            }
            goto draw_icon;
        }

        /* three IPA stat bars, in the binary's physical order A, B, C */
        IPA(0x48, 0, 0x30, 0xe, 6);   /* Adrenaline */
        IPA(0x50, 6, 0x3e, 0xf, 7);   /* Perception */
        IPA(0x4c, 3, 0x4c, 0xd, 5);   /* Intelligence */

        /* vertical health bar (entity[0x14] word) */
        hi = *(short *)(p + 0x14) * 36 / 16;
        if (g_df48[i * 0xb + 9] != hi) {
            FUN_0003feb3(HUD_X(i) + 0x34,
                         HUD_Y(i) + 6 + (0x24 - (int)g_df48[i * 0xb + 9]),
                         6, g_df48[i * 0xb + 9], 0);
            if (hi > 0) {
                FUN_0003feb3(HUD_X(i) + 0x34, HUD_Y(i) + 6 + (0x24 - hi), 6, hi, 0xc);
                g_df48[i * 0xb + 9] = (unsigned char)hi;
            }
        }

    draw_icon:
        FUN_0001b908((int)p, *(unsigned short *)(p + 0x10),
                     HUD_X(i) + 0x20, HUD_Y(i) + 0x26);
    }

    /* second pass: auxiliary progress bars hung off the reference entity */
    {
        int base = (int)g_cur_player * 0x417;
        int ref = (int)(unsigned char)g_e551[base] + (int)g_e552[base];
        unsigned char *r = g_pool_a + ref * 0x5c;
        id = *(unsigned short *)(r + 0x3a);
        i = 0;
        while (id != 0) {
            unsigned char *node;
            int frame, w, hp, val, prev;

            if (i >= 8)
                break;
            node = g_entity_pool + id;
            frame = node[0x19];
            w = g_a73a[frame];
            if (g_frame_enable[frame] != 0) {
                hp = (int)*(short *)(node + 0x14) + 1;
                val = hp * 0x17 / (w + 1);
                if (val > 0x17)
                    val = 0x17;
                prev = g_df76[i];
                if (prev != val) {
                    if (prev > 0)
                        FUN_0003feb3(*(unsigned short *)(g_auxbar_panel + i * 0x12) + 4,
                                     *(unsigned short *)(g_auxbar_panel + i * 0x12 + 2) + 0x18,
                                     prev, 4, 8);
                    if (val > 0)
                        FUN_0003feb3(*(unsigned short *)(g_auxbar_panel + i * 0x12) + 4,
                                     *(unsigned short *)(g_auxbar_panel + i * 0x12 + 2) + 0x18,
                                     val, 4, 0xe);
                    g_df76[i] = (unsigned char)val;
                }
            }
            i++;
            id = *(unsigned short *)(node + 0x1c);
        }
    }
}
