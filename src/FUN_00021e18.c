/* FUN_00021e18 @ 0x21e18 (TRUE SIZE 1440 = 0x21e18..0x223b7 incl., ends at RET;
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
 *   g_e49c[rec]        -> funds dword (rec+0)             [refund target]
 *   g_e551[rec]        -> base pool-A slot index byte (rec+0xb5)
 *   g_e5b9[rec+j*40]   -> template entry j roll byte  (0xff = empty)
 *   g_e5ba[rec+j*40]   -> template entry j HP word
 *   g_e5bc[rec+j*40]   -> template entry j flags word
 *   g_e5be[rec+j*40]   -> template entry j spare word (cleared in the clear path)
 *   g_e5c0[rec+j*40]   -> template entry j present/slot byte (0 = absent)
 *   g_e5c1[rec+j*40+d*4]-> template entry j slot d qty word
 *   g_e5c3[rec+j*40+d*4]-> template entry j slot d kind word
 * (g_e5c0 == g_e551+0x6f, so the outer loop's present-byte read is the sibling's
 * +0x6f entry field; entry stride 40, 8 {qty,kind} slot pairs -- identical layout
 * to 0x223c8's 18x40B template block.)
 *
 * Pool-A node (node = g_8110 + (g_e551[rec] + slot - 1)*0x5c, id = node - g_810e):
 *   node+0x14 hp word (signed), +0x19 type byte, +0x1c chain link (16-bit id),
 *   +0x3a carried-item chain head (16-bit id), +0x3c kind/flags word.
 * Pool-A world record `pr` in the refund scan (0x8110..0xdd10, 256x0x5c):
 *   pr+0xb flag byte, pr+0x14 hp/count word, pr+0x1c category-flags byte,
 *   pr+0x20 owner id word, pr+0x3c kind/flags word.
 *
 * Globals: g_10b45 (byte, main-body gate: body runs iff 0), g_10afc (byte mode:
 * bit2 gates the refund scan, bit8 suppresses the slot copy in block B),
 * g_10afb / g_10afd (byte counters), g_10afe / g_10b02 (dword, final clamp),
 * g_10b16 (short current-player, reset to 0 on the reset path).
 *
 * FLOW (top): if g_10b45==0, for each of 18 template entries j whose present byte
 * g_e5c0[rec+j*40]!=0: locate the node; id = node-g_810e.
 *   (A) if g_10afc&2: scan ALL 256 pool-A records; for each whose +0x20==id and
 *       !(+0xb&1), credit funds by category (bit1->+0x32, bit8->+0x96, bit4->+0x96,
 *       bit0x10->+0x12c, all clear +0x20 & bump g_10afd), OR bit2 -> re-file the
 *       item into the first empty template entry (roll byte 0xff) with HP 0x10,
 *       flags=pr[0x3c], a fresh FUN_20c88() roll and 8 zeroed slots. Every branch
 *       ends in a compiled-out 2-arg hook (see NOTE).
 *   (B) always: if node HP (node+0x14) < 0, CLEAR template entry j (roll 0xff,
 *       HP -1, flags/slot/spare 0, 8 slots 0); else SET it from the node (HP 0x10,
 *       flags=node[0x3c]) and, unless g_10afc&8, zero its 8 slots then copy the
 *       node's carried-item chain (head +0x3a, link +0x1c) into slots 0..7:
 *       qty=item[0x14], kind=item[0x19].
 * FLOW (tail, both the g_10b45!=0 skip and the normal fall-through): if param>0 &&
 * g_10b45!=0, memcpy-reset the record  FUN_4d1db(&g_e49c[rec], g_e49c, 0x417)  and
 * g_10b16 = 0. Finally clamp: if g_10afe > g_10b02, g_10afe = g_10b02.
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
 * `-4s -oneatx -zp8 -s -zq` -> obj 1394B vs TRUE 1440B (delta -46B). TWO §3 walls:
 *
 *   WALL 1 (compiled-out 2-arg hook -- the dominant blocker). The push/push/add-esp-8
 *   with no `call` requires an empty-body STACK-parm `#pragma aux`. Both forms crash
 *   the period wcc386 9.5b: `= parm [] [];` -> fatal (empty BUILD.LOG); `= parm
 *   caller [];` -> DOSBox hang/timeout. FP_SEG-style empty pragmas only work with
 *   REGISTER parms (which emit no push). So the idiom is unreachable from portable C
 *   with this toolchain. Accounts for the bulk of the -46B (the 5 hook byte-groups
 *   plus the funds load/store split + `mov cx,di` register pinning they induce).
 *
 *   WALL 2 (scaled-index / 2D address-association tie -- §3, same family as sibling
 *   0x223c8's scaled-index lea wall). For `g_e5c0[rec + j*40]` the target computes
 *   `rec` first (EAX), spills it (`mov edx,eax`), rebuilds `40j` in EAX and PRE-ADDS
 *   (`add eax,edx`) -> `[eax+disp]`. Ours (addend-first spelling `j*40 + REC`, which
 *   already fixed a worse `[ecx + eax*8]` scale-8 fold) keeps both live in a
 *   `[base+index+disp]` modrm (scale 1, no pre-add) -- 1 instr shorter per site,
 *   ~13 sites. Neither `REC + j*40` (SIB scale-8, further) nor a persistent
 *   whole-index temp (would collapse the target's per-site 0x417*param recomputes,
 *   confirmed 13 recomputes on BOTH sides) reaches the target's pre-add-then-[eax]
 *   form. Pure encoder/allocator choice; not source-reachable.
 *
 * The remaining first-diff at 0x14 is only the entry JNZ rel32 cascading from the
 * -46B tail, not a structural divergence.
 */

extern unsigned char g_810e[];
extern unsigned char g_8110[];
extern unsigned char g_e49c[];
extern unsigned char g_e551[];
extern unsigned char g_e5b9[];
extern unsigned char g_e5ba[];
extern unsigned char g_e5bc[];
extern unsigned char g_e5be[];
extern unsigned char g_e5c0[];
extern unsigned char g_e5c1[];
extern unsigned char g_e5c3[];
extern unsigned char g_10afb;
extern unsigned char g_10afc;
extern unsigned char g_10afd;
extern unsigned int  g_10afe;
extern unsigned int  g_10b02;
extern short         g_10b16;
extern unsigned char g_10b45;

extern int  FUN_00020c88(void);
extern void FUN_0004d1db(unsigned char *dst, unsigned char *src, int n);

/* compiled-out 2-arg hook: emits arg push + cdecl cleanup, no call bytes */
#define req_hook(a, b)

#define REC (param * 0x417)

void FUN_00021e18(unsigned short param)
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

    if (g_10b45 == 0) {
        for (j = 0; j < 0x12; j++) {
            slot = g_e5c0[j * 40 + REC];
            if (slot != 0) {
                node = g_8110 + (g_e551[REC] + slot - 1) * 0x5c;
                id = (int)(node - g_810e);

                /* --- (A) refund scan over the whole pool --- */
                if (g_10afc & 2) {
                    pr = g_8110;
                    if (pr < g_8110 + 256 * 0x5c) {
                        do {
                            if ((unsigned short)id != *(unsigned short *)(pr + 0x20))
                                continue;
                            if (pr[0xb] & 1)
                                continue;
                            catf = pr[0x1c];
                            if (catf & 1) {
                                *(int *)(g_e49c + REC) += 0x32;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 8) {
                                *(int *)(g_e49c + REC) += 0x96;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 4) {
                                *(int *)(g_e49c + REC) += 0x96;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 0x10) {
                                *(int *)(g_e49c + REC) += 0x12c;
                                *(unsigned short *)(pr + 0x20) = 0;
                                g_10afd++;
                                req_hook(pr, param);
                            } else if (catf & 2) {
                                g_10afd++;
                                *(unsigned short *)(pr + 0x20) = 0;
                                if (*(short *)(pr + 0x14) >= 0) {
                                    for (d = 0; d < 0x12; d++) {
                                        if (g_e5b9[d * 40 + REC] == 0xff) {
                                            g_10afb++;
                                            *(unsigned short *)(g_e5bc + d * 40 + REC) =
                                                *(unsigned short *)(pr + 0x3c);
                                            *(unsigned short *)(g_e5ba + d * 40 + REC) = 0x10;
                                            g_e5b9[d * 40 + REC] = (unsigned char)FUN_00020c88();
                                            for (s = 0; s < 8; s++) {
                                                *(unsigned short *)(g_e5c3 + d * 40 + s * 4 + REC) = 0;
                                                *(unsigned short *)(g_e5c1 + d * 40 + s * 4 + REC) = 0;
                                            }
                                            break;
                                        }
                                    }
                                }
                                req_hook(pr, param);
                            }
                        } while ((pr += 0x5c) < g_8110 + 256 * 0x5c);
                    }
                }

                /* --- (B) write template entry j from the node --- */
                if (*(short *)(node + 0x14) < 0) {
                    /* clear entry j */
                    *(unsigned short *)(g_e5ba + j * 40 + REC) = 0xffff;
                    g_e5b9[j * 40 + REC] = 0xff;
                    *(unsigned short *)(g_e5be + j * 40 + REC) = 0;
                    *(unsigned short *)(g_e5bc + j * 40 + REC) = 0;
                    g_e5c0[j * 40 + REC] = 0;
                    for (d = 0; d < 8; d++) {
                        *(unsigned short *)(g_e5c3 + j * 40 + d * 4 + REC) = 0;
                        *(unsigned short *)(g_e5c1 + j * 40 + d * 4 + REC) = 0;
                    }
                } else {
                    *(unsigned short *)(g_e5ba + j * 40 + REC) = 0x10;
                    *(unsigned short *)(g_e5bc + j * 40 + REC) =
                        *(unsigned short *)(node + 0x3c);
                    if (!(g_10afc & 8)) {
                        for (d = 0; d < 8; d++) {
                            *(unsigned short *)(g_e5c3 + j * 40 + d * 4 + REC) = 0;
                            *(unsigned short *)(g_e5c1 + j * 40 + d * 4 + REC) = 0;
                        }
                        chain = *(unsigned short *)(node + 0x3a);
                        for (d = 0; chain != 0; ) {
                            if (d >= 8)
                                break;
                            it = g_810e + chain;
                            *(unsigned short *)(g_e5c1 + j * 40 + d * 4 + REC) =
                                *(unsigned short *)(it + 0x14);
                            *(unsigned short *)(g_e5c3 + j * 40 + d * 4 + REC) = it[0x19];
                            d++;
                            chain = *(unsigned short *)(it + 0x1c);
                        }
                    }
                }
            }
        }
    }

    /* --- tail: reset path (mutually exclusive with the main body) + clamp --- */
    if (param > 0 && g_10b45 != 0) {
        FUN_0004d1db(g_e49c + REC, g_e49c, 0x417);
        g_10b16 = 0;
    }
    if (g_10afe > g_10b02)
        g_10afe = g_10b02;
}
