/* run_mission_command @ 0x23158 -- TRUE SIZE 5280 (0x14A0), 0x23158-0x245f7.
 *
 * PARKED (cont.27). Compiles; JUMP-TABLE matches EXACTLY (59 dword entries,
 * cases 0x00-0x3a). Recipe -4s -oneatx -zp8 -s -zq. EDIT-DIST 1817 (was 1878;
 * len ours 5458 vs target 5280). Best masked score ~60%
 * (3165/5280 order-preserving bytes; positional is drift-inflated). Every case
 * body is decoded and structurally correct; the residue is the SAME register-
 * role tie-break the two smaller siblings that consume this 0x417 template are
 * parked on (reequip_squad_row 694/724, build_equip_row 1090/1188): the target keeps
 * tpl live in EDX from entry and reads `movsx eax,[edx+0xb6]` (0fbe82b6) in
 * some bodies, `movsx edx,[edx+0xb6]` (0fbe92b6) in others, `movsx eax,
 * [eax+0xb6]` (0fbe80b6) in others -- an allocator choice that varies PER BODY
 * (target 2/5/5 split of the three encodings); ours homes tpl uniformly and
 * emits 0fbe92b6 x17. No single C spelling reproduces all three, so every
 * single-node body carries a 1-2 byte reg-role delta that drifts the rest of
 * that body. Frame is 0x38 vs target 0x34 (one extra allocator slot; cosmetic,
 * no drift). LEVERS THAT LANDED: op1 loop counter `unsigned short` (target
 * narrows the counter with `mov ax,bx` -- cut +29B drift to +5B); the node
 * agent selector is `(signed char)tpl[0xb6]` NOT `(char)` (Watcom char is
 * unsigned -> byte load; target uses movsx). NEW (cont.27, -61B total):
 * (1) g_10b2e is `unsigned short` not `short` -- op01 single-player branch
 * loads it zero-extended `xor eax,eax; mov ax,[10b2e]` exactly like g_e553,
 * NOT `movsx` (-6B). (2) the op10/op30 scatter scalar `a` (=lcg_rand(0xff)) is
 * `unsigned short` not `int` -- target spills it to a WORD slot and reloads it
 * word/byte-wise; narrowing realigned the whole scatter loop body (-55B).
 * TRIED+REVERTED: `short x1,y1` adds the target's `cwde` but the truncating
 * word stores inflate length with no net edit-dist gain (distance-neutral).
 * BYTE-CLOSE (reg-role residue
 * only): op01/21, op02/22, op03..op0f, op18/38..op2f, op14/34, op19/39,
 * op0b/2b, op11/31, op16/36, op12/32, op13/33, op17/37. op10/op30 -- the RNG
 * projectile-scatter bodies (record_max seed, lcg_rand rand, FUN_00037ff8
 * tracer, two find_free_slot_15e70 spawns per shot using g_dir_dy sin /
 * g_dir_dx cos scatter) -- are now STRUCTURALLY ALIGNED with the target loop
 * (same call order, same word/byte `a` handling); the residue is stack-slot
 * index numbering plus ONE extra dword spill of `a` that the target avoids
 * (ours keeps a 32-bit copy alongside the word home). That extra slot is why
 * the frame is 0x38 vs 0x34. NEXT PASS: killing that redundant `a` spill would
 * drop the frame to 0x34 and renumber the op10/op30 slots to match; it is an
 * allocator artifact, not obviously source-reachable.
 *
 * Per-record mission/orders command interpreter. Executes one queued command
 * for record `idx`, then clears the opcode byte (consume). 59-entry jump table
 * at 0x23068 (cases 0x00-0x3a, default 0x245ec); two-bank structure with 8
 * pairs sharing bodies (0x01=0x21, 0x02=0x22, 0x06=0x26, 0x0a=0x2a, 0x12=0x32,
 * 0x13=0x33, 0x17=0x37, 0x18=0x38).
 *
 * Entry: rec = g_command_recs + id*0xe (14B command record, opcode at rec[0xd]);
 * tpl = g_player_recs + id*0x417 (0x417 equip/research template row saved to [ESP+8]).
 * tpl[0xb5] = pool-A base slot (byte), tpl[0xb6] = signed agent selector.
 * Common node address = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c.
 * Squad-loop bodies walk 4 pool-A records: p in [g_pool_a + tpl[0xb5]*0x5c,
 * g_pool_a + (tpl[0xb5]+4)*0x5c). Dominant callee entity_aim_helper (aim/step).
 */
extern unsigned char g_command_recs[];
extern unsigned char g_player_recs[];
extern unsigned char g_pool_a[];
extern unsigned char g_entity_pool[];
extern short g_num_players;
extern short g_cur_player;
extern unsigned short g_10b2e;
extern unsigned short g_e553;
extern unsigned char g_player_owner[];
extern unsigned char g_in_mission;
extern unsigned char g_radar_detail;
extern unsigned char *g_10ae0;
extern char **g_map_cols;
extern short g_dir_dx[];
extern short g_dir_dy[];

extern void FUN_000229f8(int a, int b);
extern void reequip_squad_row(unsigned short row, unsigned short mode);
extern void FUN_00029d88(void);
extern void FUN_00027a88(unsigned short a);
extern int  FUN_00023038(unsigned char *p);
extern unsigned short best_weapon_select_typed(unsigned char *node, unsigned short dist, unsigned char type);
extern void entity_aim_helper(unsigned char *p, int x, int y, int z);
extern int  chain_length(unsigned char *p);
extern void dispatch_jt45(unsigned char *p);
extern unsigned short best_weapon_select(unsigned char *node, int flag);
extern void FUN_00037ff8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void record_max(unsigned char a, unsigned char b);
extern unsigned short lcg_rand(unsigned short n);
extern unsigned char *find_free_slot_15e70(int a, int b, int c);

void run_mission_command(unsigned int idx)
{
    unsigned char *rec = g_command_recs + (unsigned short)idx * 0xe;
    unsigned char *tpl = g_player_recs + (unsigned short)idx * 0x417;

    switch (rec[0xd]) {
    case 0x01: case 0x21: {
        unsigned short k;
        if (g_num_players > 1) {
            if ((unsigned short)idx == g_cur_player) {
                FUN_000229f8(0x26c, g_e553);
                for (k = 0; k < g_num_players; k++)
                    reequip_squad_row((unsigned short)k, 2);
                for (k = 0; k < g_num_players; k++)
                    g_player_owner[(unsigned short)k * 0x417] = (unsigned char)k;
            }
        } else {
            FUN_000229f8(0x26c, g_10b2e);
            reequip_squad_row((unsigned short)idx, 2);
        }
        if ((unsigned short)idx == g_cur_player)
            FUN_00029d88();
        break;
    }

    case 0x02: case 0x22: {
        if ((unsigned short)idx == g_cur_player) {
            unsigned char v = g_in_mission | 8;
            g_in_mission = v;
            g_in_mission = v & 0xfe;
        }
        if (g_radar_detail == 0)
            goto consume;
        FUN_00027a88((unsigned short)idx);
        {
            volatile int t;
            int d;
            for (d = 0; d < 0x186a0; d += 0xa)
                t += 0xa;
        }
        break;
    }

    case 0x18: case 0x38: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node))
            node[0x19] = 0;
        node[0x58] = 0;
        break;
    }

    case 0x03: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node))
            node[0x19] = 2;
        node[0x58] = 2;
        *(unsigned short *)(node + 0x2e) = *(unsigned short *)rec;
        *(unsigned short *)(node + 0x30) = *(unsigned short *)(rec + 2);
        *(unsigned short *)(node + 0x32) = *(unsigned short *)(rec + 4);
        break;
    }

    case 0x23: {
        int i;
        for (i = 0; i < 4; i++) {
            unsigned char *node = g_pool_a + (tpl[0xb5] + i) * 0x5c;
            if (node[0x1d] & 4) {
                if ((unsigned short)FUN_00023038(node))
                    node[0x19] = 2;
                node[0x58] = 2;
                *(unsigned short *)(node + 0x2e) =
                    ((i >> 1) << 7) + (*(unsigned short *)rec & 0xff00) + 0x40;
                *(unsigned short *)(node + 0x30) =
                    ((i & 1) << 7) + (*(unsigned short *)(rec + 2) & 0xff00) + 0x40;
                *(unsigned short *)(node + 0x32) = *(unsigned short *)(rec + 4);
            }
        }
        break;
    }

    case 0x3a: {
        int i;
        for (i = 0; i < 4; i++) {
            unsigned char *node = g_pool_a + (tpl[0xb5] + i) * 0x5c;
            if (node[0x1d] & 4) {
                if ((unsigned short)FUN_00023038(node))
                    node[0x19] = 2;
                node[0x58] = 2;
                *(unsigned short *)(node + 0x2e) =
                    ((i >> 1) << 7) + ((2 * (i >> 1) - 1) << 9)
                    + (*(unsigned short *)rec & 0xff00) + 0x40;
                *(unsigned short *)(node + 0x30) =
                    ((i & 1) << 7) + ((2 * (i & 1) - 1) << 9)
                    + (*(unsigned short *)(rec + 2) & 0xff00) + 0x40;
                *(unsigned short *)(node + 0x32) = *(unsigned short *)(rec + 4);
            }
        }
        break;
    }

    case 0x04: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node))
            node[0x19] = 4;
        node[0x58] = 4;
        *(unsigned short *)(node + 0x2c) = *(unsigned short *)rec;
        break;
    }

    case 0x24: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                if ((unsigned short)FUN_00023038(p))
                    p[0x19] = 4;
                p[0x58] = 4;
                *(unsigned short *)(p + 0x2c) = *(unsigned short *)rec;
            }
        }
        break;
    }

    case 0x05: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node))
            node[0x19] = 5;
        if ((short)chain_length(node) >= 8)
            node[0x58] = node[0x19];
        else
            node[0x58] = 9;
        *(unsigned short *)(node + 0x2c) = *(unsigned short *)rec;
        break;
    }

    case 0x25: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                if ((unsigned short)FUN_00023038(p))
                    p[0x19] = 5;
                if ((short)chain_length(p) >= 8)
                    p[0x58] = p[0x19];
                else
                    p[0x58] = 9;
                *(unsigned short *)(p + 0x2c) = *(unsigned short *)rec;
            }
        }
        break;
    }

    case 0x06: case 0x26:
        tpl[0xb6] = rec[0];
        break;

    case 0x07: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        unsigned char *q = g_entity_pool + *(unsigned short *)rec;
        node[0x46] = 0;
        *(unsigned short *)(node + 0x44) = (unsigned short)(q - g_entity_pool);
        break;
    }

    case 0x27: {
        unsigned char *carried = g_entity_pool + *(unsigned short *)rec;
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        unsigned char *p;
        node[0x46] = 0;
        *(unsigned short *)(node + 0x44) = (unsigned short)(carried - g_entity_pool);
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                if (p != g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c) {
                    unsigned char *nn = g_entity_pool + best_weapon_select_typed(p, 0, carried[0x19]);
                    if (nn[0x19] == carried[0x19])
                        *(unsigned short *)(p + 0x44) = (unsigned short)(nn - g_entity_pool);
                    p[0x46] = 0;
                }
            }
        }
        break;
    }

    case 0x08: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        unsigned char *p, *end;
        unsigned short link = *(unsigned short *)(node + 0x44);
        if (link != 0) {
            unsigned char *carried = g_entity_pool + link;
            if ((unsigned short)FUN_00023038(node) &&
                *(short *)(carried + 0x14) >= 0)
                entity_aim_helper(node, *(short *)rec, *(short *)(rec + 2),
                             *(short *)(rec + 4));
        }
        if ((*(unsigned short *)(node + 0x1c) & 0x1002) != 2)
            goto consume;
        end = g_10ae0;
        if (g_pool_a < end) {
            p = g_pool_a;
            do {
                if (*(unsigned short *)(p + 0x20) ==
                    (unsigned short)(node - g_entity_pool))
                    entity_aim_helper(p, *(short *)rec, *(short *)(rec + 2),
                                 *(short *)(rec + 4));
                p += 0x5c;
            } while (p < g_10ae0);
        }
        break;
    }

    case 0x28: {
        unsigned char *node;
        int cur_id;
        node = g_pool_a + tpl[0xb5] * 0x5c;
        cur_id = node - g_entity_pool;
        for (; node < g_pool_a + (tpl[0xb5] + 4) * 0x5c;
             node += 0x5c, cur_id += 0x5c) {
            unsigned short link = *(unsigned short *)(node + 0x44);
            if (link != 0) {
                unsigned char *carried = g_entity_pool + link;
                if ((node[0x1d] & 4) && *(short *)(carried + 0x14) >= 0 &&
                    (unsigned short)FUN_00023038(node))
                    entity_aim_helper(node, *(short *)rec, *(short *)(rec + 2),
                                 *(short *)(rec + 4));
            }
            if ((*(unsigned short *)(node + 0x1c) & 0x1002) == 2) {
                unsigned char *p;
                if (g_10ae0 > g_pool_a) {
                    p = g_pool_a;
                    do {
                        if (*(unsigned short *)(p + 0x20) == (unsigned short)cur_id)
                            entity_aim_helper(p, *(short *)rec, *(short *)(rec + 2),
                                         *(short *)(rec + 4));
                        p += 0x5c;
                    } while (p < g_10ae0);
                }
            }
        }
        break;
    }

    case 0x09: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node))
            node[0x19] = 5;
        node[0x58] = 6;
        *(unsigned short *)(node + 0x2c) = *(unsigned short *)rec;
        dispatch_jt45(node);
        break;
    }

    case 0x29: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                if ((unsigned short)FUN_00023038(p))
                    p[0x19] = 5;
                p[0x58] = 6;
                *(unsigned short *)(p + 0x2c) = *(unsigned short *)rec;
                dispatch_jt45(p);
            }
        }
        break;
    }

    case 0x0a: case 0x2a: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if ((unsigned short)FUN_00023038(node)) {
            node[0x19] = 0xa;
            node[0x58] = 0xa;
            *(unsigned short *)(node + 0x44) = *(unsigned short *)rec;
            node[0x46] = 0;
            dispatch_jt45(node);
        }
        break;
    }

    case 0x0b: {
        int off = ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        *(unsigned short *)(g_pool_a + off + 0x44) = 0;
        g_pool_a[off + 0x46] = 0;
        break;
    }

    case 0x2b: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                *(unsigned short *)(p + 0x44) = 0;
                p[0x46] = 0;
            }
        }
        break;
    }

    case 0x19: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        int col;
        if (*(unsigned short *)(node + 0x24) == 0)
            goto consume;
        col = (((*(short *)(node + 6) % 0x6000) / 0x100) << 7)
              + ((*(short *)(node + 4) & 0xff00) / 0x100);
        if ((unsigned char)*(g_map_cols[col] + *(short *)(node + 8) / 0x80) == 2)
            goto consume;
        node[0x19] = 7;
        node[0x58] = 7;
        node[0xa] &= 0xf7;
        break;
    }

    case 0x39: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && *(unsigned short *)(p + 0x24) != 0) {
                int col = (((*(short *)(p + 6) % 0x6000) / 0x100) << 7)
                          + ((*(short *)(p + 4) & 0xff00) / 0x100);
                if ((unsigned char)*(g_map_cols[col] + *(short *)(p + 8) / 0x80) != 2) {
                    p[0x19] = 7;
                    p[0x58] = 7;
                    p[0xa] &= 0xf7;
                }
            }
        }
        break;
    }

    case 0x14: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if (node[0xb] & 1)
            goto consume;
        if (*(short *)rec != -1)
            node[0x49] = rec[0];
        if (*(short *)(rec + 2) != -1)
            node[0x4d] = rec[2];
        if (*(short *)(rec + 4) == -1)
            goto consume;
        node[0x51] = rec[4];
        break;
    }

    case 0x34: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && !(p[0xb] & 1)) {
                if (*(short *)rec != -1)
                    p[0x49] = rec[0];
                if (*(short *)(rec + 2) != -1)
                    p[0x4d] = rec[2];
                if (*(short *)(rec + 4) != -1)
                    p[0x51] = rec[4];
            }
        }
        break;
    }

    case 0x0c: {
        unsigned char *node = g_pool_a + (tpl[0xb5] + *(unsigned short *)rec) * 0x5c;
        if (node[0xb] & 1)
            goto consume;
        node[0x49] = rec[2];
        break;
    }

    case 0x2c: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && !(p[0xb] & 1))
                p[0x49] = rec[2];
        }
        break;
    }

    case 0x0d: {
        unsigned char *node = g_pool_a + (tpl[0xb5] + *(unsigned short *)rec) * 0x5c;
        if (node[0xb] & 1)
            goto consume;
        node[0x4d] = rec[2];
        break;
    }

    case 0x2d: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && !(p[0xb] & 1))
                p[0x4d] = rec[2];
        }
        break;
    }

    case 0x0e: {
        unsigned char *node = g_pool_a + (tpl[0xb5] + *(unsigned short *)rec) * 0x5c;
        if (node[0xb] & 1)
            goto consume;
        node[0x51] = rec[2];
        break;
    }

    case 0x2e: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && !(p[0xb] & 1))
                p[0x51] = rec[2];
        }
        break;
    }

    case 0x0f: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if (node[0xa] & 8)
            goto consume;
        node[0x19] = 0x1c;
        node[0x58] = 0x1c;
        *(unsigned short *)(node + 0x42) = 0xffff;
        break;
    }

    case 0x2f: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if ((p[0x1d] & 4) && !(p[0xa] & 8) && p[0x19] == 0) {
                p[0x19] = 0x1c;
                p[0x58] = 0x1c;
                *(unsigned short *)(p + 0x42) = 0xffff;
            }
        }
        break;
    }

    case 0x10: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        int nid, k, n, i;
        unsigned short a;
        unsigned char *n2;
        if (node[0xb] & 1)
            goto consume;
        k = (*(unsigned short *)(node + 0x3c) & 0x60) >> 5;
        if (k == 0)
            goto consume;
        if (*(short *)(node + 0x14) < 0)
            goto consume;
        {
            unsigned short cid = *(unsigned short *)(node + 0x24);
            while (cid != 0) {
                unsigned char *c = g_entity_pool + cid;
                if (c[0x18] == 2) {
                    *(unsigned short *)(c + 0x14) = 0xfff6;
                    break;
                }
                cid = *(unsigned short *)(c + 0x1e);
            }
        }
        nid = node - g_entity_pool;
        *(unsigned short *)(node + 0x14) = 0xfff6;
        n = k * 2;
        for (i = 0; i < n; i++) {
            int x1, y1, d;
            record_max(0x16, 0x7f);
            a = lcg_rand(0xff);
            FUN_00037ff8(*(short *)(node + 4), *(short *)(node + 6),
                         *(short *)(node + 8) + 0x80, (unsigned char)a, 0,
                         lcg_rand(9) + 1, k << 8, 0x2c);
            d = lcg_rand(0xff);
            y1 = *(short *)(node + 6) + ((int)g_dir_dy[d] * lcg_rand(a) >> 8);
            d = lcg_rand(0xff);
            x1 = *(short *)(node + 4) + ((int)g_dir_dx[d] * lcg_rand(a) >> 8);
            n2 = find_free_slot_15e70(x1, y1, *(short *)(node + 8));
            if (n2 != 0) {
                n2[0x19] = 0xa;
                *(unsigned short *)(n2 + 0x1c) = (unsigned short)nid;
            }
            d = lcg_rand(0xff);
            y1 = *(short *)(node + 6) + ((int)g_dir_dy[d] * lcg_rand(a) >> 8);
            d = lcg_rand(0xff);
            x1 = *(short *)(node + 4) + ((int)g_dir_dx[d] * lcg_rand(a) >> 8);
            n2 = find_free_slot_15e70(x1, y1, *(short *)(node + 8));
            if (n2 != 0) {
                n2[0x19] = 7;
                *(unsigned short *)(n2 + 0x1c) = (unsigned short)nid;
            }
        }
        break;
    }

    case 0x30: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            int nid, k, n, i;
            unsigned short a;
            unsigned char *n2;
            if (!(p[0x1d] & 4) || (p[0xb] & 1))
                continue;
            k = (*(unsigned short *)(p + 0x3c) & 0x60) >> 5;
            if (k == 0)
                continue;
            if (*(short *)(p + 0x14) < 0)
                continue;
            nid = p - g_entity_pool;
            *(unsigned short *)(p + 0x14) = 0xfff6;
            n = k * 2;
            for (i = 0; i < n; i++) {
                int x1, y1, d;
                record_max(0x16, 0x7f);
                a = lcg_rand(0xff);
                FUN_00037ff8(*(short *)(p + 4), *(short *)(p + 6),
                             *(short *)(p + 8) + 0x80, (unsigned char)a, 0,
                             lcg_rand(9) + 1, k << 8, 0x2c);
                d = lcg_rand(0xff);
                y1 = *(short *)(p + 6) + ((int)g_dir_dy[d] * lcg_rand(a) >> 8);
                d = lcg_rand(0xff);
                x1 = *(short *)(p + 4) + ((int)g_dir_dx[d] * lcg_rand(a) >> 8);
                n2 = find_free_slot_15e70(x1, y1, *(short *)(p + 8));
                if (n2 != 0) {
                    n2[0x19] = 0xa;
                    *(unsigned short *)(n2 + 0x1c) = (unsigned short)nid;
                }
                d = lcg_rand(0xff);
                y1 = *(short *)(p + 6) + ((int)g_dir_dy[d] * lcg_rand(a) >> 8);
                d = lcg_rand(0xff);
                x1 = *(short *)(p + 4) + ((int)g_dir_dx[d] * lcg_rand(a) >> 8);
                n2 = find_free_slot_15e70(x1, y1, *(short *)(p + 8));
                if (n2 != 0) {
                    n2[0x19] = 7;
                    *(unsigned short *)(n2 + 0x1c) = (unsigned short)nid;
                }
            }
        }
        break;
    }

    case 0x11: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        unsigned short r = best_weapon_select(node, 0);
        node[0x49] = 0xff;
        node[0x4d] = 0xff;
        node[0x51] = 0xff;
        *(unsigned short *)(node + 0x44) = r;
        break;
    }

    case 0x31: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                unsigned short r = best_weapon_select(p, 0);
                p[0x49] = 0xff;
                p[0x4d] = 0xff;
                p[0x51] = 0xff;
                *(unsigned short *)(p + 0x44) = r;
            }
        }
        break;
    }

    case 0x16: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if (node[0xa] & 8)
            goto consume;
        *(unsigned short *)(node + 0x44) = best_weapon_select_typed(node, 0, rec[0]);
        break;
    }

    case 0x36: {
        unsigned char *p;
        for (p = g_pool_a + tpl[0xb5] * 0x5c;
             p < g_pool_a + (tpl[0xb5] + 4) * 0x5c; p += 0x5c) {
            if (p[0x1d] & 4) {
                unsigned char *nn = g_entity_pool + *(unsigned short *)(p + 0x44);
                if (nn >= (unsigned char *)0x11670 &&
                    *(short *)(nn + 0x14) < 0 && p[0x19] != 0xa)
                    *(unsigned short *)(p + 0x44) = best_weapon_select_typed(p, 0, nn[0x19]);
            }
        }
        break;
    }

    case 0x12: case 0x32: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        node[0x19] = 0x1f;
        node[0x58] = 0x1f;
        *(unsigned short *)(node + 0x2a) = *(unsigned short *)rec;
        *(unsigned short *)(node + 0x2c) = *(unsigned short *)(rec + 2);
        break;
    }

    case 0x13: case 0x33: {
        unsigned char *node = g_pool_a + ((signed char)tpl[0xb6] + tpl[0xb5]) * 0x5c;
        if (node[0xa] & 8)
            goto consume;
        node[0x19] = 0x20;
        node[0x58] = 0x20;
        *(unsigned short *)(node + 0x2a) = *(unsigned short *)rec;
        *(unsigned short *)(node + 0x2c) = *(unsigned short *)(rec + 2);
        break;
    }

    case 0x17: case 0x37:
        reequip_squad_row((unsigned short)idx, 2);
        break;

    case 0x00: case 0x15: case 0x1a: case 0x1b: case 0x1c: case 0x1d:
    case 0x1e: case 0x1f: case 0x20: case 0x35:
    default:
        break;
    }

consume:
    rec[0xd] = 0;
}
