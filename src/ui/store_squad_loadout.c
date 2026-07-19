/* store_squad_loadout @ 0x21e18 (TRUE SIZE 1440 = 0x21e18..0x223b7 incl., ends at RET;
 * matches the manifest 1440. This was a SEPARATE function the headless sweep had
 * merged into 0x21658; own prologue push ebx/esi/edi/ebp + sub esp,0xc, one
 * ushort param, shared 4-pop epilogue.)
 *
 * SQUAD "STORE / RETURN LOADOUT" routine -- the INVERSE of sibling 0x223c8
 * (which reads an equip template and CREATES the squad's carried items). Here we
 * WALK each squad node's carried-item chain and WRITE it back into the per-player
 * equip-template table, crediting refunds for world items owned by the node.
 * Same 0x417 template family as 0x223c8 / 0x12da8 / 0x23158 / 0x21658.
 *
 * rec = 0x417*param (stride 1047, indexed by the passed player index `param`).
 * The record's field bases (all the same 1047-byte record, different C symbols):
 *   g_player_recs[rec]        -> funds dword (rec+0)             [refund target]
 *   g_agent_slots[rec]        -> base pool-A slot index byte (rec+0xb5)
 *   g_squad_id[rec+j*40]   -> template entry j roll byte  (0xff = empty)
 *   g_e5ba[rec+j*40]   -> template entry j HP word
 *   g_e5bc[rec+j*40]   -> template entry j flags word
 *   g_e5be[rec+j*40]   -> template entry j spare word (cleared in the clear path)
 *   g_squad_slot[rec+j*40]   -> template entry j present/slot byte (0 = absent)
 *   g_equip_qty[rec+j*40+d*4]-> template entry j slot d qty word
 *   g_equip_kind[rec+j*40+d*4]-> template entry j slot d kind word
 * (g_squad_slot == g_agent_slots+0x6f, so the outer loop's present-byte read is the sibling's
 * +0x6f entry field; entry stride 40, 8 {qty,kind} slot pairs -- identical layout
 * to 0x223c8's 18x40B template block.)
 *
 * Pool-A node (node = g_pool_a + (g_agent_slots[rec] + slot - 1)*0x5c, id = node - g_entity_pool):
 *   node+0x14 hp word (signed), +0x19 type byte, +0x1c chain link (16-bit id),
 *   +0x3a carried-item chain head (16-bit id), +0x3c kind/flags word.
 * Pool-A world record `pr` in the refund scan (0x8110..0xdd10, 256x0x5c):
 *   pr+0xb flag byte, pr+0x14 hp/count word, pr+0x1c category-flags byte,
 *   pr+0x20 owner id word, pr+0x3c kind/flags word.
 *
 * Globals: g_radar_detail (byte, main-body gate: body runs iff 0), g_in_mission (byte mode:
 * bit2 gates the refund scan, bit8 suppresses the slot copy in block B),
 * g_10afb / g_10afd (byte counters), g_10afe / g_10b02 (dword, final clamp),
 * g_cur_player (short current-player, reset to 0 on the reset path).
 *
 * FLOW (top): if g_radar_detail==0, for each of 18 template entries j whose present byte
 * g_squad_slot[rec+j*40]!=0: locate the node; id = node-g_entity_pool.
 *   (A) if g_in_mission&2: scan ALL 256 pool-A records; for each whose +0x20==id and
 *       !(+0xb&1), credit funds by category (bit1->+0x32, bit8->+0x96, bit4->+0x96,
 *       bit0x10->+0x12c, all clear +0x20 & bump g_10afd), OR bit2 -> re-file the
 *       item into the first empty template entry (roll byte 0xff) with HP 0x10,
 *       flags=pr[0x3c], a fresh FUN_20c88() roll and 8 zeroed slots. Every branch
 *       ends in a compiled-out 2-arg hook (see NOTE).
 *   (B) always: if node HP (node+0x14) < 0, CLEAR template entry j (roll 0xff,
 *       HP -1, flags/slot/spare 0, 8 slots 0); else SET it from the node (HP 0x10,
 *       flags=node[0x3c]) and, unless g_in_mission&8, zero its 8 slots then copy the
 *       node's carried-item chain (head +0x3a, link +0x1c) into slots 0..7:
 *       qty=item[0x14], kind=item[0x19].
 * FLOW (tail, both the g_radar_detail!=0 skip and the normal fall-through): if param>0 &&
 * g_radar_detail!=0, memcpy-reset the record  FUN_4d1db(&g_player_recs[rec], g_player_recs, 0x417)  and
 * g_cur_player = 0. Finally clamp: if g_10afe > g_10b02, g_10afe = g_10b02.
 *
 * NOTE (the "hook"): every one of the 5 refund/re-file branches ends with a
 * 2-argument cdecl call whose callee emits ZERO code in this build -- the bytes are
 * `push <player>; ...; push <pr>; add esp,8` with NO `call` (a compiled-out
 * telemetry/debug hook: arg eval + cdecl cleanup only). Modelled here as the no-op
 * macro `req_hook`. It could NOT be reproduced (see WALL 1) so those 5 sites are
 * absent from our object; that also lets Watcom keep the funds update in-place and
 * `param` in EDX where the target splits load/store and pins `param` in ECX
 * (`mov cx,di`) to feed the `push ecx`.
 *
 * STATUS: full readable-C decode, PARKED. Structure fully recovered; scored with
 * `-4s -oneatx -zp8 -s -zq` -> obj 1332B vs TRUE 1440B (delta -108B), EDIT-DIST 439
 * (~69.5% masked-byte match, up from the earlier 702 / 51%). ONE remaining §3 wall:
 *
 *   WALL 1 (compiled-out 2-arg hook -- the dominant blocker). The push/push/add-esp-8
 *   with no `call` requires an empty-body STACK-parm `#pragma aux`. Both forms crash
 *   the period wcc386 9.5b: `= parm [] [];` -> fatal (empty BUILD.LOG); `= parm
 *   caller [];` -> DOSBox hang/timeout. FP_SEG-style empty pragmas only work with
 *   REGISTER parms (which emit no push). So the idiom is unreachable from portable C
 *   with this toolchain. This is now essentially the WHOLE remaining gap: the 5 hook
 *   byte-groups, the funds load/store split (target `mov r,[funds]; add r,imm; mov
 *   [funds],r` vs our folded `add [funds],imm`), the per-branch `g_10afd` reload the
 *   hook's register clobber forces (target reloads in every branch; without the hook
 *   Watcom hoists our load+inc once before the dispatch), and the `mov cx,di` param
 *   pinning that feeds `push ecx`. All trace to the missing hook.
 *
 *   The former "WALL 2" (2D address association) turned out NOT to be a wall. The
 *   correct spelling is RECORD-FIRST: `g_squad_slot[REC + j*40]` (REC == 0x417*param).
 *   That makes Watcom compute REC into a register and KEEP IT LIVE, reusing it for the
 *   sibling `g_agent_slots[REC]` access one instruction later -- exactly the target's
 *   REC-reuse cascade (target holds REC in EDX and reuses `mov al,[edx+0xe551]`; ours
 *   holds it in ECX and reuses `mov al,[ecx+..]`). The earlier addend-first spelling
 *   `j*40 + REC` defeated that reuse and RE-COMPUTED REC per access, which is what held
 *   the score at 702. Switching every 2D index to REC-first dropped EDIT-DIST 702->439.
 *   The only residue at these sites is pure encoder choice: the target pre-adds into
 *   `[eax+disp]`, ours folds `j*40` into a scale-8 SIB `[ecx+eax*8+disp]` (1 byte
 *   shorter/site) -- both keep REC live and recompute it per site (13 recomputes each),
 *   so the data flow matches; only the address encoding differs. Not source-reachable
 *   and no longer material next to WALL 1.
 *
 * The remaining first-diff at 0x14 is only the entry JNZ rel32 cascading from the
 * -108B tail (all WALL 1), not a structural divergence.
 */

extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char g_player_recs[];
extern unsigned char g_agent_slots[];
extern unsigned char g_squad_id[];
extern unsigned char g_e5ba[];
extern unsigned char g_e5bc[];
extern unsigned char g_e5be[];
extern unsigned char g_squad_slot[];
extern unsigned char g_equip_qty[];
extern unsigned char g_equip_kind[];
extern unsigned char g_10afb;
extern unsigned char g_in_mission;
extern unsigned char g_10afd;
extern unsigned int  g_10afe;
extern unsigned int  g_10b02;
extern short         g_cur_player;
extern unsigned char g_radar_detail;

extern int  keyboard_state_machine(void);
extern void copy_bytes(unsigned char *dst, unsigned char *src, int n);

/* compiled-out 2-arg hook: emits arg push + cdecl cleanup, no call bytes */
#define req_hook(a, b)

#define REC (param * 0x417)

void store_squad_loadout(unsigned short param)
{
    unsigned char *node;
    unsigned char *pr;
    unsigned char *it;
    int id;
    unsigned short j;
    unsigned short d;
    unsigned short s;
    unsigned short catf;
    unsigned short chain;
    unsigned char  slot;

    if (g_radar_detail == 0) {
        for (j = 0; j < 0x12; j++) {
            slot = g_squad_slot[REC + j * 40];
            if (slot != 0) {
                node = g_pool_a + (g_agent_slots[REC] + slot - 1) * 0x5c;
                id = (int)(node - g_entity_pool);

                /* --- (A) refund scan over the whole pool --- */
                if (g_in_mission & 2) {
                    pr = g_pool_a;
                    if (pr < g_pool_a + 256 * 0x5c) {
                        do {
                            if ((unsigned short)id != *(unsigned short *)(pr + 0x20))
                                continue;
                            if (pr[0xb] & 1)
                                continue;
                            catf = pr[0x1c];
                            if (catf & 1) {
                                *(int *)(g_player_recs + REC) += 0x32;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 8) {
                                *(int *)(g_player_recs + REC) += 0x96;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 4) {
                                *(int *)(g_player_recs + REC) += 0x96;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 0x10) {
                                *(int *)(g_player_recs + REC) += 0x12c;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 2) {
                                g_10afd++;
                                *(unsigned short *)(pr + 0x20) = 0;
                                if (*(short *)(pr + 0x14) >= 0) {
                                    for (d = 0; d < 0x12; d++) {
                                        if (g_squad_id[REC + d * 40] == 0xff) {
                                            g_10afb++;
                                            *(unsigned short *)(g_e5bc + REC + d * 40) =
                                                *(unsigned short *)(pr + 0x3c);
                                            *(unsigned short *)(g_e5ba + REC + d * 40) = 0x10;
                                            g_squad_id[REC + d * 40] = (unsigned char)keyboard_state_machine();
                                            for (s = 0; s < 8; s++) {
                                                *(unsigned short *)(g_equip_kind + REC + d * 40 + s * 4) = 0;
                                                *(unsigned short *)(g_equip_qty + REC + d * 40 + s * 4) = 0;
                                            }
                                            break;
                                        }
                                    }
                                }
                                req_hook(pr, param);
                            }
                        } while ((pr += 0x5c) < g_pool_a + 256 * 0x5c);
                    }
                }

                /* --- (B) write template entry j from the node --- */
                if (*(short *)(node + 0x14) < 0) {
                    /* clear entry j */
                    *(unsigned short *)(g_e5ba + REC + j * 40) = 0xffff;
                    g_squad_id[REC + j * 40] = 0xff;
                    *(unsigned short *)(g_e5be + REC + j * 40) = 0;
                    *(unsigned short *)(g_e5bc + REC + j * 40) = 0;
                    g_squad_slot[REC + j * 40] = 0;
                    for (d = 0; d < 8; d++) {
                        *(unsigned short *)(g_equip_kind + REC + j * 40 + d * 4) = 0;
                        *(unsigned short *)(g_equip_qty + REC + j * 40 + d * 4) = 0;
                    }
                } else {
                    *(unsigned short *)(g_e5ba + REC + j * 40) = 0x10;
                    *(unsigned short *)(g_e5bc + REC + j * 40) =
                        *(unsigned short *)(node + 0x3c);
                    if (!(g_in_mission & 8)) {
                        for (d = 0; d < 8; d++) {
                            *(unsigned short *)(g_equip_kind + REC + j * 40 + d * 4) = 0;
                            *(unsigned short *)(g_equip_qty + REC + j * 40 + d * 4) = 0;
                        }
                        chain = *(unsigned short *)(node + 0x3a);
                        for (d = 0; chain != 0; ) {
                            if (d >= 8)
                                break;
                            it = g_entity_pool + chain;
                            *(unsigned short *)(g_equip_qty + REC + j * 40 + d * 4) =
                                *(unsigned short *)(it + 0x14);
                            *(unsigned short *)(g_equip_kind + REC + j * 40 + d * 4) = it[0x19];
                            d++;
                            chain = *(unsigned short *)(it + 0x1c);
                        }
                    }
                }
            }
        }
    }

    /* --- tail: reset path (mutually exclusive with the main body) + clamp --- */
    if (param > 0 && g_radar_detail != 0) {
        copy_bytes(g_player_recs + REC, g_player_recs, 0x417);
        g_cur_player = 0;
    }
    if (g_10afe > g_10b02)
        g_10afe = g_10b02;
}
