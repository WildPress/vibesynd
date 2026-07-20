/* draw_tactical_map @ 0x19608 -- TRUE SIZE 3474 (0x19608..0x1a399).
 *
 * The tactical-map / radar renderer. Draws the full-screen overview map centred
 * on the currently-followed agent: terrain tiles, then a coloured blip for every
 * person/vehicle in view, then the selected-agent pulse ring and the animated
 * mission-objective markers.
 *
 * PROPOSED NAME: draw_tactical_map(agent, zoom)  -- the full-screen radar/overview.
 *
 * Signature: draw_tactical_map(unsigned char *agent, unsigned short zoom)
 *   agent = the pool-A node the view is centred on (its world x/y at +4/+6);
 *   zoom  = pixels-per-tile scale (read zero-extended: unsigned short).
 *
 * STATUS: ours 3402B vs target 3474B (delta -72; was -236). The -300 code gap
 * was a RECONSTRUCTION GAP, now closed: the Phase-3 mission-marker loop has TWO
 * distinct drawing tails, not one shared tail. We had collapsed both branches
 * onto a single tail; the target dispatches otype through a jump table and gives
 * each objective class its own inlined tail (see Phase-3 fix below). That added
 * ~164B and removed an 84-instruction target-only block; structural match ratio
 * 0.634 -> 0.668. Residue is now a codegen-tie floor (below).
 *
 * Phase-3 fix (the missing ~300 bytes): switch(otype).
 *  - Fixed-coordinate marker (otype 0x10, coords at rec+8/rec+0xa) keeps the FULL
 *    tail: g_marker_anim = 3 on wrap, range-checked g_offscreen_obj indicator, and
 *    the on-screen zoom*3 ring vs off-screen g_marker_anim ring split.
 *  - Node-tracking markers (otype 1/2/3/5/0xf, id at rec+6) get a SEPARATE simpler
 *    tail: g_marker_anim = 0 on wrap (not 3), indicator fired on g_offscreen_obj
 *    ALONE (no range check), and only ever the g_marker_anim ring (no zoom*3 draw).
 *  The target emits these as two full copies; ours now does too.
 *
 * Key source fixes that realigned it (all faithful, not byte-tricks):
 *  - span / tileX0 / tileY0 / row / col / row2 / col2 are `short`. That stops
 *    Watcom hoisting the `tileY0+span+1` / `tileX0+span+1` loop bounds into stack
 *    temps: the target RECOMPUTES the bound every iteration from 16-bit reloads
 *    (movsx WORD). This alone dropped the frame from 0x688 to ~0x650 (target
 *    0x644) and realigned both nested loops.
 *  - zoom is `unsigned short`: the target loads it zero-extended (xor r,r / mov
 *    r16) then divides signed (idiv) -- exactly the unsigned-short->int promotion.
 *  - subX / subY are `short` (target reloads them movsx WORD).
 *  - The three sel_x/sel_y/sel_r = -1 initialisers hoisted to the top (target
 *    emits them first, sharing a single edx=-1).
 *  - Phase-1 terrain: the (x,y,w,h) args to fill_rect are written inline in
 *    each switch case, NOT via shared sx/sy temps -- the target recomputes them
 *    per case (each case is a separate jump-table block, no cross-block CSE).
 *  - Phase-2: tsx/tsy folded directly into blipX/blipY (no separate temps).
 *  - Phase-1 index: row term written first, col term second, matching the
 *    target's evaluation order.
 *  - Tail blip-draw pass 1: `chr` is `short` (blip[i*6+4]+1). The target keeps it
 *    16-bit and sign-extends (`movsx edx,dx`) right before pushing it to
 *    draw_filled_shape; as `int` ours emitted a plain 32-bit `lea`, misaligning the
 *    push block. `short` -> masked edit-distance 2000 -> 1968.
 *    (TESTED-AND-REVERTED, faithful-but-worse: `short owner=-1` reproduces the
 *    target's `mov edx,0xffff` at 0x19b61, and `unsigned char arg` reproduces its
 *    `and dl,0xf; and edx,0xff` byte-widen at 0x19fe2 -- each is byte-for-byte more
 *    faithful in isolation, but every combination that includes `owner` or pairs
 *    `chr`+`arg` REGRESSES total edit distance via register-allocation cascade
 *    (owner 2003, arg 1982, chr+arg 2010, owner+chr 2017). `chr` alone is the min.)
 *
 * REMAINING GAP (codegen ties, not source-reachable):
 *  0. fill_rect cross-jump merge: the target tail-merges its Phase-2 fill_rect
 *     call sites -- the type-2 case pushes its 5 args then `jmp` INTO the type-4
 *     `call fill_rect; add esp,0x14` (one physical call at 0x8f8 serves both), and
 *     the two colour-0xc danger dots likewise share 0x898. Watcom emits only 2
 *     physical phase-2 fill_rect calls; ours emits 4 (behaviourally identical, all
 *     the right calls, just unmerged). -oneatx cross-jumping fired for the target
 *     but not for our arg-setup shapes; ~16-24B, not reliably source-forced.
 *  1. Register allocation: the target keeps `count` in EDI and `zoom` in ECX;
 *     ours lands `count` in EBX and `zoom` in EBX. `count` in EBX vs EDI changes
 *     the SIB/disp of every blip[count*6+n] store across the large phase-2 type-1
 *     colour block -- the single biggest residual. Not controllable from C.
 *  2. g_map_cols triangle: Watcom strength-reduces `col<<8` into a stack induction
 *     variable (extra slots, frame 0x650 vs 0x644); the target recomputes it from
 *     `col` each iteration. Same 9.5 codegen, opposite decision.
 *  3. Heavy signed fixed-point: ~14 IDIV / cwd-shl-sbb-sar div-by-256 and
 *     div-by-zoom chains whose accumulator/remainder register ties differ.
 *  4. Three co-located jump tables Watcom emits in .text BEFORE the function proper
 *     in the original (tables at 0x19564, code at 0x19608); our build folds ~0x40
 *     bytes of table into the head of the symbol, a fixed structural mismatch:
 *       - table1 @ 0x19564, 16 entries, index g_tile_flags[tile]  (terrain shape)
 *       - table2 @ 0x195a4,  6 entries, index node[0x18]     (blip by type)
 *       - table3 @ 0x195bc, 17 entries, index objective type  (HUD markers)
 *     (lefix rule: literal L in `jmp CS:[eax*4+L]` -> manifest L+0xd748.)
 * Compiles (-4s -oneatx -zp8 -s -zq); ours 3402B vs target 3474B. First diff at
 * 0x3c is inside that co-located jump-table head, not a logic divergence.
 *
 * ---- Algorithm ----
 * Prologue projects the camera: the view is a (128/zoom + 2) square of tiles
 * around the agent's tile; subX/subY are the sub-tile scroll remainders.
 *
 * Phase 1 (terrain): for every tile (row,col) in view, look up its ground tile
 * through the g_map_cols column table, map it through g_tile_flags to a terrain shape
 * 0..15, and draw that shape as a filled quad via fill_rect(x,y,w,h,colour)
 * (colour 0xf / 7 / 0xa / 0 per shape class).
 *
 * Phase 2 (blips): for every tile, walk the spatial-grid entity chain
 * (g_grid_heads[cell] -> id -> node[0] link, capped at 300). Skip hidden nodes and,
 * for owned agents, foreign-team ones. Project each node to a blip position and
 * dispatch on node[0x18]: type 2 -> a filled square (colour 0xc); type 4 -> a
 * 2x2 dot (colour 4, only if g_e395); type 1 -> an agent blip appended to the
 * local buffer with a friend/foe/target colour. The followed agent's own blip
 * position is remembered (sel_x/sel_y/sel_r).
 *
 * Tail: draw_vline / draw_hline recentre the radar on the followed agent;
 * two passes over the blip buffer draw each blip (shadow then sprite) via the
 * matched draw_filled_shape; draw_circle draws the selected-agent pulse ring; and
 * the phase-3 loop scans 8 objective slots (0x1be3a, stride 14) and draws the
 * first active one's animated marker (fixed-coord type 0x10, or node-tracking
 * types 1/2/3/5/0xf) via sum_of_squares_call (length) + draw_circle, returning.
 */
extern char **g_map_cols;              /* map column-pointer table               */
extern unsigned char *g_tile_flags;     /* tile -> terrain-shape table            */
extern unsigned short g_grid_heads[];     /* 128x128 spatial-grid head ids          */
extern unsigned char g_entity_pool[];     /* pool-A base - 2 (index by id)          */
extern unsigned char g_pool_a[];     /* pool-A base                            */
extern unsigned char g_player_owner[];     /* per-player template: team byte         */
extern unsigned char g_blip_colours[];     /* team -> blip colour table              */
extern unsigned char g_objectives[];    /* 8 objective slots, stride 14           */
extern short g_cur_player;              /* current/owning player                  */
extern short g_target_countdown;              /* selected-target countdown (pulse anim) */
extern short g_pulse_ring_r;               /* pulse-ring base radius                 */
extern short g_marker_anim;               /* objective-marker animation phase       */
extern unsigned char g_radar_detail;      /* radar detail-mode flag                 */
extern unsigned char g_in_mission;      /* "in mission" flag                      */
extern unsigned char g_offscreen_obj;      /* off-screen-objective indicator flag    */
extern unsigned char g_e395;       /* show projectile/danger blips           */
extern unsigned char g_e396;       /* target-marker colour offset            */
extern unsigned char g_e397;       /* objective-marker colour phase          */

extern void fill_rect(int x, int y, int w, int h, int colour); /* fill quad */
extern void draw_vline(int a, int b, int c, int d);   /* radar x recentre    */
extern void draw_hline(int a, int b, int c, int d);   /* radar y recentre    */
extern void draw_filled_shape(int x, int y, int chr, int colour);      /* draw blip  */
extern void draw_circle(int x, int y, int r, int colour);        /* draw ring  */
extern int  sum_of_squares_call(int a, int b);                 /* marker anim length  */
extern void record_max(int a, int b);                 /* off-screen indicator*/

void draw_tactical_map(unsigned char *agent, unsigned short zoom)
{
    unsigned char blip[0x600];      /* 256 x {u16 x, u16 y, u8 char, u8 col}  */
    int sel_x = -1, sel_y = -1, sel_r = -1;   /* followed agent's blip / mark  */
    short span   = 0x80 / zoom;     /* half-extent of the visible tile square */
    int half   = 0x8000 / (zoom * 2);
    int camx   = *(short *)(agent + 4) - half;
    int camy   = *(short *)(agent + 6) - half;
    int scale  = 0x100 / zoom;
    short subX = ((unsigned char)camx) / scale;   /* sub-tile scroll x        */
    short subY = ((unsigned char)camy) / scale;   /* sub-tile scroll y        */
    short tileX0 = (short)camx / 0x100;           /* top-left visible tile     */
    short tileY0 = (short)camy / 0x100;
    int count  = 0;                 /* blips appended to the buffer            */
    short row, col, row2, col2;
    char **base;
    char **slot;

    /* ---- Phase 1: terrain tiles ---- */
    for (row = tileY0; row <= tileY0 + span + 1; row++) {
        if (row >= 0x60) break;
        if (row < 0) continue;
        for (col = tileX0; col <= tileX0 + span + 1; col++) {
            int shape, index;
            unsigned char tile;
            if (col >= 0x80) break;
            if (col < 0) continue;

            index = (((short)((row << 8) % 0x6000) / 0x100) << 7)
                  + ((short)((col << 8) & 0xff00) / 0x100);
            base = g_map_cols;
            slot = base + index;
            tile = *(unsigned char *)((int)*slot);   /* ground tile byte */
            shape = g_tile_flags[tile];
            if (shape > 0xf)
                continue;

            switch (shape) {
            case 1: case 2: case 3: case 4: case 0xd:
                fill_rect((col - tileX0) * zoom - subX,
                             (row - tileY0) * zoom - subY, zoom, zoom, 0xf);
                break;
            case 5: case 0xe:
                fill_rect((col - tileX0) * zoom - subX,
                             (row - tileY0) * zoom - subY, zoom, zoom, 7);
                break;
            case 6: case 7: case 8: case 9: case 0xb: case 0xf:
                fill_rect((col - tileX0) * zoom - subX,
                             (row - tileY0) * zoom - subY, zoom, zoom, 0xa);
                break;
            case 0xa:
                fill_rect((col - tileX0) * zoom - subX,
                             (row - tileY0) * zoom - subY, zoom, zoom, 0);
                break;
            default:            /* 0, 0xc: nothing */
                break;
            }
        }
    }

    /* ---- Phase 2: entity blips ---- */
    for (row2 = tileY0; row2 <= tileY0 + span + 1; row2++) {
        if (row2 >= 0x60) break;
        if (row2 < 0) continue;
        for (col2 = tileX0; col2 <= tileX0 + span + 1; col2++) {
            unsigned short id;
            int cnt;
            if (col2 >= 0x80) break;
            if (col2 < 0) continue;

            id = g_grid_heads[col2 + (row2 << 7)];
            if (id == 0)
                continue;

            cnt = 0;
            do {
                unsigned char *node = g_entity_pool + id;
                int radius, blipX, blipY, type;

                if (node[0xb] & 1)
                    goto next_node;                       /* hidden */
                if (node[0xa] & 1) {                       /* owned-agent flag */
                    int rec = (int)((node - g_pool_a) / 0x5c) / 8;
                    if (rec != g_cur_player)
                        goto next_node;                   /* not our team */
                }

                radius = zoom / 2;
                if (radius < 1)
                    radius = 1;
                blipX = (col2 - tileX0) * zoom - subX
                      + (*(unsigned char *)(node + 4) * zoom) / 0x100;
                blipY = (row2 - tileY0) * zoom - subY
                      + (*(unsigned char *)(node + 6) * zoom) / 0x100;
                type = node[0x18];
                if (type > 5)
                    goto next_node;

                switch (type) {
                case 2:     /* filled square blip */
                    fill_rect(blipX - radius / 2, blipY - radius / 2,
                                 radius, radius, 0xc);
                    break;
                case 4:     /* small dot (projectile / danger) */
                    if (g_e395 != 0)
                        fill_rect(blipX - 1, blipY - 1, 2, 2, 4);
                    break;
                case 1: {   /* agent blip -> buffer with friend/foe colour */
                    short tgt = *(short *)(node + 0x20);
                    int owner = -1;
                    if (node[0x1c] & 2)
                        owner = (int)((node - g_pool_a) / 0x5c) / 8;

                    if (g_radar_detail != 0) {
                        if ((node[0x1c] & 8) && tgt == 0) {
                            *(short *)(blip + count * 6) = (short)blipX;
                            *(short *)(blip + count * 6 + 2) = (short)blipY;
                            blip[count * 6 + 4] = 3;
                            blip[count * 6 + 5] = 4;
                            count++;
                        } else if ((node[0x1c] & 4) && tgt == 0) {
                            *(short *)(blip + count * 6) = (short)blipX;
                            *(short *)(blip + count * 6 + 2) = (short)blipY;
                            blip[count * 6 + 4] = 3;
                            blip[count * 6 + 5] = 0xf;
                            count++;
                        } else {
                            if (tgt != 0)
                                owner = (int)((g_entity_pool + tgt - g_pool_a) / 0x5c) / 8;
                            if (owner == g_cur_player || g_target_countdown > 0) {
                                if (tgt == -1)
                                    goto dot45;
                                *(short *)(blip + count * 6) = (short)blipX;
                                *(short *)(blip + count * 6 + 2) = (short)blipY;
                                blip[count * 6 + 4] = 3;
                                blip[count * 6 + 5] = g_blip_colours[g_player_owner[owner * 0x417]];
                                count++;
                                break;
                            }
                          dot45:
                            if (tgt != -1)
                                break;
                            if (g_e395 == 0)
                                break;
                            fill_rect(blipX - 1, blipY - 1, 2, 2, 0xc);
                        }
                    } else {
                        if ((node[0x1c] & 8) && tgt == 0) {
                            *(short *)(blip + count * 6) = (short)blipX;
                            *(short *)(blip + count * 6 + 2) = (short)blipY;
                            blip[count * 6 + 4] = 3;
                            blip[count * 6 + 5] = 4;
                            count++;
                        } else if ((node[0x1c] & 4) && tgt == 0) {
                            *(short *)(blip + count * 6) = (short)blipX;
                            *(short *)(blip + count * 6 + 2) = (short)blipY;
                            blip[count * 6 + 4] = 3;
                            blip[count * 6 + 5] = 0xf;
                            count++;
                        } else {
                            if (tgt != 0)
                                owner = (int)((g_entity_pool + tgt - g_pool_a) / 0x5c) / 8;
                            if (tgt == -1) {
                                if (g_e395 == 0)
                                    break;
                                fill_rect(blipX - 1, blipY - 1, 2, 2, 0xc);
                                break;
                            }
                            *(short *)(blip + count * 6) = (short)blipX;
                            *(short *)(blip + count * 6 + 2) = (short)blipY;
                            blip[count * 6 + 4] = 3;
                            if (owner == g_cur_player) {
                                if (tgt != 0 && (node[0x1c] & 0x20))
                                    blip[count * 6 + 5] = (unsigned char)(g_e396 + 0xb);
                                else
                                    blip[count * 6 + 5] = 0xb;
                            } else {
                                blip[count * 6 + 5] = 0xe;
                            }
                            count++;
                        }
                    }

                    if (node == agent) {   /* remember our own blip position */
                        sel_x = blipX;
                        sel_y = blipY;
                        sel_r = radius;
                    }
                    break;
                }
                default:        /* 0, 3, 5: nothing */
                    break;
                }

              next_node:
                id = *(unsigned short *)node;   /* grid chain link */
                cnt++;
            } while (cnt < 0x12c && id != 0);
        }
    }

    /* ---- Tail: recentre radar, draw blips, pulse ring ---- */
    draw_vline(0, 0x80, sel_x, 0);
    draw_hline(0, 0x80, sel_y, 0);

    {
        int i;
        for (i = 0; i < count; i++) {
            short chr = blip[i * 6 + 4] + 1;
            int arg = (g_e395 != 0) ? 0 : (blip[i * 6 + 5] + 8) & 0xf;
            draw_filled_shape(*(short *)(blip + i * 6), *(short *)(blip + i * 6 + 2),
                         chr, arg);
        }
        for (i = 0; i < count; i++) {
            draw_filled_shape(*(short *)(blip + i * 6), *(short *)(blip + i * 6 + 2),
                         blip[i * 6 + 4], blip[i * 6 + 5]);
        }
    }

    if (sel_r != -1 && g_radar_detail != 0) {
        int ring = (int)g_pulse_ring_r / 4 + g_pulse_ring_r;   /* g_pulse_ring_r * 5/4 */
        if (g_target_countdown > 0) {
            int r = 90 * (ring - g_target_countdown) / ring;
            draw_circle(sel_x, sel_y, r, 0xc);
        }
    }

    g_target_countdown = -1;
    if (g_radar_detail != 0)
        return;
    if (g_in_mission != 1)
        return;

    /* ---- Phase 3: mission-objective markers ----
     * switch(otype) dispatches through the 17-entry jump table.  The two live
     * objective classes each carry their OWN drawing tail (the target does NOT
     * share them): the fixed-coordinate marker (0x10) gets the full tail with
     * the on-screen zoom*3 ring and the range-checked off-screen indicator; the
     * node-tracking markers (1/2/3/5/0xf) get the simpler tail that resets the
     * animation to 0, fires the indicator on g_offscreen_obj alone, and only ever
     * draws the g_marker_anim ring. */
    {
        int cx;
        for (cx = 0; cx < 8; cx++) {
            unsigned char *rec = g_objectives + cx * 0xe;
            unsigned short otype;
            int mx, my, scale2, off, len;

            if (*(int *)rec != 0)
                continue;
            otype = *(unsigned short *)(rec + 4);
            if (otype > 0x10)
                continue;

            switch (otype) {
            case 0x10:      /* fixed-coordinate objective (rec+8 / rec+0xa) */
                scale2 = 0x100 / zoom;
                mx = (*(short *)(rec + 8) - *(short *)(agent + 4)) / scale2 + 0x40;
                my = (*(short *)(rec + 0xa) - *(short *)(agent + 6)) / scale2 + 0x40;

                off = sum_of_squares_call(0x40 - mx, 0x40 - my);
                g_marker_anim += zoom;
                len = (off & 0xffff) + zoom * 4;
                if (g_marker_anim > len) {
                    g_marker_anim = 3;
                    if (g_offscreen_obj != 0 &&
                        !(mx >= 0 && mx < 0x80 && my >= 0 && my < 0x80))
                        record_max(0x11, 0x7f);     /* off-screen: edge marker */
                }
                if (mx >= 0 && mx < 0x80 && my >= 0 && my < 0x80) {
                    int colour = (0xe - g_e397 * 8) & 0xff;
                    draw_circle(mx, my, zoom * 3, colour);   /* on-screen marker */
                } else {
                    draw_circle(mx, my, g_marker_anim, 0xc); /* off-screen ring  */
                }
                return;

            case 1: case 2: case 3: case 5: case 0xf: {
                /* node-tracking objective: id at rec+6 */
                unsigned char *onode =
                    g_entity_pool + *(unsigned short *)(rec + 6);
                scale2 = 0x100 / zoom;
                mx = (*(short *)(onode + 4) - *(short *)(agent + 4)) / scale2 + 0x40;
                my = (*(short *)(onode + 6) - *(short *)(agent + 6)) / scale2 + 0x40;

                off = sum_of_squares_call(0x40 - mx, 0x40 - my);
                g_marker_anim += zoom;
                len = (off & 0xffff) + zoom * 4;
                if (g_marker_anim > len) {
                    g_marker_anim = 0;
                    if (g_offscreen_obj != 0)
                        record_max(0x11, 0x7f);
                }
                draw_circle(mx, my, g_marker_anim, 0xc);
                return;
            }

            default:        /* 0, 4, 6..0xe: stop scanning */
                return;
            }
        }
    }
}
