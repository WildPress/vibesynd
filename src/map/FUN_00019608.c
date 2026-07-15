/* FUN_00019608 @ 0x19608 -- TRUE SIZE 3474 (0x19608..0x1a399).
 *
 * The tactical-map / radar renderer. Draws the full-screen overview map centred
 * on the currently-followed agent: terrain tiles, then a coloured blip for every
 * person/vehicle in view, then the selected-agent pulse ring and the animated
 * mission-objective markers.
 *
 * Signature: FUN_00019608(unsigned char *agent, short zoom)
 *   agent = the pool-A node the view is centred on (its world x/y at +4/+6);
 *   zoom  = pixels-per-tile scale.
 *
 * PARKED (decode-only) -- multiply walled, byte-parity is not reachable:
 *  1. g_map_cols column-table lookup sits in the register-triangle wall (base load /
 *     lea index-scale / tile deref ordering) that parks 0x2d5b8 / 0x28ec8.
 *  2. Heavy signed fixed-point: ~14 IDIV / cwd-shl-sbb-sar div-by-256 and
 *     div-by-zoom chains whose accumulator/remainder register ties are the
 *     encoding tie-break wall.
 *  3. A ~0x600-byte local blip buffer (256 six-byte records {x:u16, y:u16,
 *     char:u8, colour:u8}) laid out at the BOTTOM of the 0x644 frame, below ~20
 *     spilled locals. Portable C cannot reproduce that exact stack layout, and
 *     every [ESP+disp] in the function keys off it.
 *  4. Three co-located jump tables (Watcom emits them in .text before the code):
 *       - table1 @ 0x19564, 16 entries, index g_tile_flags[tile]  (terrain shape)
 *       - table2 @ 0x195a4,  6 entries, index node[0x18]     (blip by type)
 *       - table3 @ 0x195bc, 17 entries, index objective type  (HUD markers)
 *     (lefix rule: literal L in `jmp CS:[eax*4+L]` -> manifest L+0xd748.)
 * Compiles (-4s -oneatx -zp8 -s -zq); ours 3279B vs target 3474B, first diff at
 * 0x3c -- in the prologue fixed-point projection (the div-by-256 / div-by-zoom
 * IDIV chains): ours emits a `lea` (8d) where the target has an accumulator
 * form (24). All three jump tables split/masked cleanly. Structure faithful;
 * residue is the g_map_cols triangle + IDIV ties + the ~0x600-byte stack blip array.
 *
 * ---- Algorithm ----
 * Prologue projects the camera: the view is a (128/zoom + 2) square of tiles
 * around the agent's tile; subX/subY are the sub-tile scroll remainders.
 *
 * Phase 1 (terrain): for every tile (row,col) in view, look up its ground tile
 * through the g_map_cols column table, map it through g_tile_flags to a terrain shape
 * 0..15, and draw that shape as a filled quad via FUN_0003fb40(x,y,w,h,colour)
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
 * Tail: FUN_0003f4b4 / FUN_0003f636 recentre the radar on the followed agent;
 * two passes over the blip buffer draw each blip (shadow then sprite) via the
 * matched FUN_00018d18; FUN_00019318 draws the selected-agent pulse ring; and
 * the phase-3 loop scans 8 objective slots (0x1be3a, stride 14) and draws the
 * first active one's animated marker (fixed-coord type 0x10, or node-tracking
 * types 1/2/3/5/0xf) via FUN_00014c58 (length) + FUN_00019318, returning.
 */
extern char **g_map_cols;              /* map column-pointer table               */
extern unsigned char *g_tile_flags;     /* tile -> terrain-shape table            */
extern unsigned short g_grid_heads[];     /* 128x128 spatial-grid head ids          */
extern unsigned char g_entity_pool[];     /* pool-A base - 2 (index by id)          */
extern unsigned char g_pool_a[];     /* pool-A base                            */
extern unsigned char g_e4ab[];     /* per-player template: team byte         */
extern unsigned char g_b46a[];     /* team -> blip colour table              */
extern unsigned char g_1be3a[];    /* 8 objective slots, stride 14           */
extern short g_cur_player;              /* current/owning player                  */
extern short g_10b30;              /* selected-target countdown (pulse anim) */
extern short g_a74e;               /* pulse-ring base radius                 */
extern short g_3ef4;               /* objective-marker animation phase       */
extern unsigned char g_10b45;      /* radar detail-mode flag                 */
extern unsigned char g_10afc;      /* "in mission" flag                      */
extern unsigned char g_10b4b;      /* off-screen-objective indicator flag    */
extern unsigned char g_e395;       /* show projectile/danger blips           */
extern unsigned char g_e396;       /* target-marker colour offset            */
extern unsigned char g_e397;       /* objective-marker colour phase          */

extern void FUN_0003fb40(int x, int y, int w, int h, int colour); /* fill quad */
extern void FUN_0003f4b4(int a, int b, int c, int d);   /* radar x recentre    */
extern void FUN_0003f636(int a, int b, int c, int d);   /* radar y recentre    */
extern void FUN_00018d18(int x, int y, int chr, int colour);      /* draw blip  */
extern void FUN_00019318(int x, int y, int r, int colour);        /* draw ring  */
extern int  FUN_00014c58(int a, int b);                 /* marker anim length  */
extern void FUN_00035f28(int a, int b);                 /* off-screen indicator*/

void FUN_00019608(unsigned char *agent, short zoom)
{
    unsigned char blip[0x600];      /* 256 x {u16 x, u16 y, u8 char, u8 col}  */
    int span   = 0x80 / zoom;       /* half-extent of the visible tile square */
    int half   = 0x8000 / (zoom * 2);
    int camx   = *(short *)(agent + 4) - half;
    int camy   = *(short *)(agent + 6) - half;
    int scale  = 0x100 / zoom;
    int subX   = ((unsigned char)camx) / scale;   /* sub-tile scroll x        */
    int subY   = ((unsigned char)camy) / scale;   /* sub-tile scroll y        */
    int tileX0 = (short)camx / 0x100;             /* top-left visible tile     */
    int tileY0 = (short)camy / 0x100;
    int count  = 0;                 /* blips appended to the buffer            */
    int sel_x = -1, sel_y = -1, sel_r = -1;   /* followed agent's blip / mark  */
    int row, col, row2, col2;
    char **base;
    char **slot;

    /* ---- Phase 1: terrain tiles ---- */
    for (row = tileY0; row <= tileY0 + span + 1; row++) {
        if (row >= 0x60) break;
        if (row < 0) continue;
        for (col = tileX0; col <= tileX0 + span + 1; col++) {
            int shape, index, sx, sy;
            unsigned char tile;
            if (col >= 0x80) break;
            if (col < 0) continue;

            index = ((short)((col << 8) & 0xff00) / 0x100)
                  + (((short)((row << 8) % 0x6000) / 0x100) << 7);
            base = g_map_cols;
            slot = base + index;
            tile = *(unsigned char *)((int)*slot);   /* ground tile byte */
            shape = g_tile_flags[tile];
            if (shape > 0xf)
                continue;

            sx = (col - tileX0) * zoom - subX;
            sy = (row - tileY0) * zoom - subY;
            switch (shape) {
            case 1: case 2: case 3: case 4: case 0xd:
                FUN_0003fb40(sx, sy, zoom, zoom, 0xf);
                break;
            case 5: case 0xe:
                FUN_0003fb40(sx, sy, zoom, zoom, 7);
                break;
            case 6: case 7: case 8: case 9: case 0xb: case 0xf:
                FUN_0003fb40(sx, sy, zoom, zoom, 0xa);
                break;
            case 0xa:
                FUN_0003fb40(sx, sy, zoom, zoom, 0);
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
                int tsx, tsy;

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
                tsx = (col2 - tileX0) * zoom - subX;
                tsy = (row2 - tileY0) * zoom - subY;
                blipX = tsx + (*(unsigned char *)(node + 4) * zoom) / 0x100;
                blipY = tsy + (*(unsigned char *)(node + 6) * zoom) / 0x100;
                type = node[0x18];
                if (type > 5)
                    goto next_node;

                switch (type) {
                case 2:     /* filled square blip */
                    FUN_0003fb40(blipX - radius / 2, blipY - radius / 2,
                                 radius, radius, 0xc);
                    break;
                case 4:     /* small dot (projectile / danger) */
                    if (g_e395 != 0)
                        FUN_0003fb40(blipX - 1, blipY - 1, 2, 2, 4);
                    break;
                case 1: {   /* agent blip -> buffer with friend/foe colour */
                    short tgt = *(short *)(node + 0x20);
                    int owner = -1;
                    if (node[0x1c] & 2)
                        owner = (int)((node - g_pool_a) / 0x5c) / 8;

                    if (g_10b45 != 0) {
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
                            if (owner == g_cur_player || g_10b30 > 0) {
                                if (tgt == -1)
                                    goto dot45;
                                *(short *)(blip + count * 6) = (short)blipX;
                                *(short *)(blip + count * 6 + 2) = (short)blipY;
                                blip[count * 6 + 4] = 3;
                                blip[count * 6 + 5] = g_b46a[g_e4ab[owner * 0x417]];
                                count++;
                                break;
                            }
                          dot45:
                            if (tgt != -1)
                                break;
                            if (g_e395 == 0)
                                break;
                            FUN_0003fb40(blipX - 1, blipY - 1, 2, 2, 0xc);
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
                                FUN_0003fb40(blipX - 1, blipY - 1, 2, 2, 0xc);
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
    FUN_0003f4b4(0, 0x80, sel_x, 0);
    FUN_0003f636(0, 0x80, sel_y, 0);

    {
        int i;
        for (i = 0; i < count; i++) {
            int chr = blip[i * 6 + 4] + 1;
            int arg = (g_e395 != 0) ? 0 : (blip[i * 6 + 5] + 8) & 0xf;
            FUN_00018d18(*(short *)(blip + i * 6), *(short *)(blip + i * 6 + 2),
                         chr, arg);
        }
        for (i = 0; i < count; i++) {
            FUN_00018d18(*(short *)(blip + i * 6), *(short *)(blip + i * 6 + 2),
                         blip[i * 6 + 4], blip[i * 6 + 5]);
        }
    }

    if (sel_r != -1 && g_10b45 != 0) {
        int ring = (int)g_a74e / 4 + g_a74e;   /* g_a74e * 5/4 */
        if (g_10b30 > 0) {
            int r = 90 * (ring - g_10b30) / ring;
            FUN_00019318(sel_x, sel_y, r, 0xc);
        }
    }

    g_10b30 = -1;
    if (g_10b45 != 0)
        return;
    if (g_10afc != 1)
        return;

    /* ---- Phase 3: mission-objective markers ---- */
    {
        int cx;
        for (cx = 0; cx < 8; cx++) {
            unsigned char *rec = g_1be3a + cx * 0xe;
            unsigned short otype;
            int mx, my, scale2, off, len;

            if (*(int *)rec != 0)
                continue;
            otype = *(unsigned short *)(rec + 4);
            if (otype > 0x10)
                continue;

            if (otype == 0x10) {
                /* fixed-coordinate objective (rec+8 / rec+0xa) */
                scale2 = 0x100 / zoom;
                mx = (*(short *)(rec + 8) - *(short *)(agent + 4)) / scale2 + 0x40;
                my = (*(short *)(rec + 0xa) - *(short *)(agent + 6)) / scale2 + 0x40;
            } else if (otype == 1 || otype == 2 || otype == 3 ||
                       otype == 5 || otype == 0xf) {
                /* node-tracking objective: id at rec+6 */
                unsigned char *onode =
                    g_entity_pool + *(unsigned short *)(rec + 6);
                scale2 = 0x100 / zoom;
                mx = (*(short *)(onode + 4) - *(short *)(agent + 4)) / scale2 + 0x40;
                my = (*(short *)(onode + 6) - *(short *)(agent + 6)) / scale2 + 0x40;
            } else {
                /* 4, 6..0xe: stop scanning */
                return;
            }

            /* advance the shared dashed-line animation phase */
            off = FUN_00014c58(0x40 - mx, 0x40 - my);
            g_3ef4 += zoom;
            len = (off & 0xffff) + zoom * 4;
            if (g_3ef4 > len) {
                g_3ef4 = 3;
                if (g_10b4b != 0 &&
                    !(mx >= 0 && mx < 0x80 && my >= 0 && my < 0x80))
                    FUN_00035f28(0x11, 0x7f);     /* off-screen: edge marker */
            }

            if (mx >= 0 && mx < 0x80 && my >= 0 && my < 0x80) {
                int colour = (0xe - g_e397 * 8) & 0xff;
                FUN_00019318(mx, my, zoom * 3, colour);   /* on-screen marker */
            } else {
                FUN_00019318(mx, my, g_3ef4, 0xc);        /* off-screen ring  */
            }
            return;
        }
    }
}
