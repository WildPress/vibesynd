/* entity_state_dispatch @ 0x133a8 -- entity command/animation state-machine dispatcher.
 * TRUE SIZE 1528 (0x5f8, 0x133a8-0x1399f incl; manifest 1528 is CORRECT).
 *
 * PARKED (register-allocation + spill-slot wall). Full decode below; the C is
 * byte-faithful in STRUCTURE (best -oneatx: 1524B code vs 1528B, delta -4;
 * first code diff at the entry frame). All residual diffs are equal-length
 * register-role swaps that cascade from the entry, NOT missing/wrong C:
 *   - ENTRY CASCADE: target loads hnd=p1[0xc] into EBP (callee-saved,
 *     coalesced with param_3's later EBP home); ours picks scratch EDX. That
 *     one choice flips the param_1[0]*15 index temp (target xor edx;mov dl vs
 *     ours movzx edi) and every downstream reg role in the entry block. No
 *     source spelling reliably flips a coalescing preference (cont.25 wall map
 *     class 1/2).
 *   - FRAME/SPILL COUNT: target sub esp,0x14 (5 dword slots, cur at [esp+8]);
 *     ours sub esp,0x10 (4 slots, cur at [esp+4]). Cases 9/10 spill one more
 *     distinct value in the original than -oneatx keeps live -- spill-slot
 *     assignment is allocator-internal (cont.21 wall).
 *   - TAIL: the arrival check reuses known-equal regs via `xor edx,esi`
 *     (target) where ours re-issues `xor esi,esi` -- the encoding/known-clear
 *     tie-break (cont.25 class 1). Equal length.
 * Recipes tried: -4s -oneatx (best, 1524/1528, first diff entry frame);
 * -4s -or (1475B, worse, first diff 0x34). Not decl-order/spelling reachable.
 *
 * TWO co-located jump tables sit directly BEFORE the code (Ghidra addr =
 * table value + 0xd748):
 *   main  @ manifest 0x13354 (jmp CS:[EAX*4+0x5c0c]), 13 entries, state = p1[3]
 *     0:0x134c4 1:0x134d2 2:0x134f9 3/5/7:tail 4:0x135f8 6:0x13615 8:0x138b0
 *     9:0x13784 10:0x1367b 11:0x138d7 12:0x1391a
 *   inner @ manifest 0x13388 (jmp CS:[EAX*4+0x5c40]), 8 entries, on rec[7]-3
 *     idx3(=6):0x135da(default); all others:0x135bc; JA-default:0x135da
 *   code physical case order: 0,1,2,4,6,10,9,8,11,12 (= source order).
 *
 * Model: p1 = command/state struct; p4 = anim-request struct (ushort*);
 * hnd = p1[0xc] (pointer, re-read); cur = g_entity_pool + hnd[0x3a] (pool-A node,
 * cached in a slot); rec = g_1beb2 + p1[0]*15 (15-byte record); tnode =
 * g_entity_pool + p1[0xa] (pool-A node). See docs/object-model.md.
 */
extern unsigned char g_entity_pool[];
extern unsigned char g_1beb2[];
extern unsigned char g_c358[];
extern unsigned short g_item_max_qty[];
extern unsigned short g_a6c2[];
extern void FUN_00023158(unsigned int);
extern unsigned short lcg_rand(int);
extern unsigned char *pool_list_dispatch(unsigned char *, unsigned char *, int);
extern char forward_if_flag(unsigned short *, unsigned char *, unsigned char *, unsigned char);
extern char kill_credit_eligible(unsigned char *, unsigned char *, unsigned char);
extern void palette_flash3(short *, int, int, unsigned char, unsigned char *);
extern void reset_flash_palette(unsigned char *, unsigned short *, unsigned char);
extern void FUN_00013c98(unsigned short *, unsigned char *, unsigned char,
                         unsigned char, unsigned char *, unsigned char *);
extern void FUN_00014588(unsigned char *);
extern unsigned char *walk_15byte_chain(unsigned char *);
extern char FUN_000141f8(unsigned char *, unsigned char *, unsigned char,
                         unsigned char, unsigned short *);
extern void FUN_00014828(unsigned char *);
extern void FUN_000149e8(unsigned char *, unsigned char *);

void entity_state_dispatch(unsigned char *param_1, unsigned char param_2, unsigned char param_3,
                  unsigned short *param_4, char param_5)
{
    unsigned char *cur;
    unsigned char *rec;
    unsigned char *tnode;
    unsigned char *hnd;
    unsigned char *n;
    short q;

    hnd = *(unsigned char **)(param_1 + 0xc);
    if (hnd[0xb] & 1) {
        param_1[3] = 5;
        return;
    }
    rec = g_1beb2 + *param_1 * 0xf;
    cur = g_entity_pool + *(unsigned short *)(hnd + 0x3a);
    *(short *)(cur + 0x14) = (short)((int)(unsigned short)g_item_max_qty[cur[0x19]] / 4);
    *param_4 = (unsigned short)param_2;
    *((unsigned char *)param_4 + 0xd) = 6;
    FUN_00023158((unsigned short)param_3);

    hnd = *(unsigned char **)(param_1 + 0xc);
    if (hnd[0xc] & 0x40) {
        if (*(short *)(hnd + 0x14) < 2) {
            if (lcg_rand(0x64) < 5) {
                param_1[3] = 4;
                goto dispatch;
            }
        } else {
            unsigned char old = param_1[3];
            *((unsigned char *)param_4 + 0xd) = 0x18;
            FUN_00023158((unsigned short)param_3);
            param_1[2] = 1;
            *(unsigned short *)(param_1 + 0xa) =
                *(unsigned short *)(*(unsigned char **)(param_1 + 0xc) + 0x16);
            if (old != 0xc) {
                param_1[3] = 0xc;
                param_1[8] = 0xa;
                param_1[9] = old;
            }
            param_1[5] = 0;
        }
    }
    if (param_5 == 0 && param_1[3] != 0xc)
        return;

dispatch:
    tnode = g_entity_pool + *(unsigned short *)(param_1 + 0xa);
    if (param_1[3] > 0xc)
        goto tail;
    switch (param_1[3]) {
    case 0:
        FUN_00014588(param_1);
        goto tail;
    case 1:
        param_1[3] = 2;
        FUN_00013c98(param_4, rec, param_3, param_2, param_1, cur);
        goto tail;
    case 2:
        if (rec[6] & 0x10) {
            unsigned char *t = g_c358 + param_1[7] * 2;
            param_1[3] = 0;
            if (t[0] != 0)
                t[1] = 0;
            rec = walk_15byte_chain(rec);
            goto tail;
        }
        if (FUN_000141f8(rec, param_1, param_2, param_3, param_4)) {
            if (*(unsigned short *)rec == 0)
                goto tail;
            param_1[0] = rec[0];
            rec = g_1beb2 + *(unsigned short *)rec * 0xf;
            if (rec[7] == 4)
                FUN_00014828(param_1);
            else
                FUN_000149e8(param_1, rec);
            FUN_00013c98(param_4, rec, param_3, param_2, param_1, cur);
            goto tail;
        }
        switch (rec[7]) {
        case 3: case 4: case 5: case 7: case 8: case 9: case 10:
            FUN_00013c98(param_4, rec, param_3, param_2, param_1, cur);
            /* fall through */
        default:
            FUN_000141f8(rec, param_1, param_2, param_3, param_4);
        }
        goto tail;
    case 4:
        *((unsigned char *)param_4 + 0xd) = 0x10;
        FUN_00023158((unsigned short)param_3);
        goto tail;
    case 6:
        if (forward_if_flag(param_4, tnode, param_1, param_3)) {
            q = (short)((int)(unsigned short)g_a6c2[cur[0x19]] * 9 / 10);
            n = pool_list_dispatch(*(unsigned char **)(param_1 + 0xc), tnode, q);
            param_1[3] = (n == tnode) + 9;
        }
        goto tail;
    case 10:
        if (!forward_if_flag(param_4, tnode, param_1, param_3))
            goto tail;
        q = (short)((int)(unsigned short)g_a6c2[cur[0x19]] * 9 / 10);
        n = pool_list_dispatch(*(unsigned char **)(param_1 + 0xc), tnode, q);
        if (n != 0) {
            if (n == tnode) {
                if (kill_credit_eligible(param_1, n, param_3))
                    palette_flash3(param_4, (int)*(unsigned char **)(param_1 + 0xc),
                                 (int)tnode, param_3, param_1);
                else
                    reset_flash_palette(param_1, param_4, param_3);
            } else if (kill_credit_eligible(param_1, n, param_3)) {
                palette_flash3(param_4, (int)*(unsigned char **)(param_1 + 0xc),
                             (int)cur, param_3, param_1);
            }
        } else if (kill_credit_eligible(param_1, tnode, param_3)) {
            param_1[3] = 9;
        } else {
            reset_flash_palette(param_1, param_4, param_3);
        }
        goto tail;
    case 9:
        if (!forward_if_flag(param_4, tnode, param_1, param_3))
            goto tail;
        if (kill_credit_eligible(param_1, tnode, param_3)) {
            q = (short)((int)(unsigned short)g_a6c2[cur[0x19]] * 9 / 10);
            n = pool_list_dispatch(*(unsigned char **)(param_1 + 0xc), tnode, q);
            if (n != 0) {
                if (n == tnode) {
                    palette_flash3(param_4, (int)*(unsigned char **)(param_1 + 0xc),
                                 (int)tnode, param_3, param_1);
                } else if (kill_credit_eligible(param_1, n, param_3)) {
                    palette_flash3(param_4, (int)*(unsigned char **)(param_1 + 0xc),
                                 (int)cur, param_3, param_1);
                }
            } else if (param_1[5] == 0) {
                param_1[5] = 1;
                *((unsigned char *)param_4 + 0xd) = 0x14;
                *param_4 = 0xff;
                param_4[1] = 0x7f;
                param_4[2] = 0x7f;
                FUN_00023158((unsigned short)param_3);
                *((unsigned char *)param_4 + 0xd) = 4;
                *param_4 = *(unsigned short *)(param_1 + 0xa);
                FUN_00023158((unsigned short)param_3);
            }
        } else {
            reset_flash_palette(param_1, param_4, param_3);
        }
        goto tail;
    case 8:
        FUN_00013c98(param_4, rec, param_3, param_2, param_1, cur);
        param_1[3] = 2;
        goto tail;
    case 11:
        if ((*(unsigned char **)(param_1 + 0xc))[0x19] != 0) {
            *((unsigned char *)param_4 + 0xd) = 0x18;
            FUN_00023158((unsigned short)param_3);
            param_1[0] = 0xff;
            param_1[2] = 0;
            param_1[5] = 0;
            param_1[6] = 0;
            *(unsigned short *)(param_1 + 0xa) = 0;
            *(int *)(param_1 + 0x10) = 0;
        }
        goto tail;
    case 12: {
        unsigned char hv = (*(unsigned char **)(param_1 + 0xc))[0x19];
        unsigned char c;
        unsigned char s;
        if (hv == 0x16 || hv == 0x12)
            break;
        c = param_1[8] - 1;
        param_1[8] = c;
        if (c != 0)
            break;
        s = param_1[9];
        param_1[3] = s;
        if (s == 6 || s == 9 || s == 0xa)
            break;
        param_1[3] = 6;
        param_1[4] = s;
        param_1[5] = c;
        break;
    }
    case 3:
    case 5:
    case 7:
    default:
        break;
    }
tail:
    {
        unsigned char *h = *(unsigned char **)(param_1 + 0xc);
        if (h[0x19] == 0 && param_1[0] != 0xff && rec[7] == 1 && rec[8] == 3) {
            if (*(short *)(h + 4) != *(unsigned short *)(rec + 9)
             || *(short *)(h + 6) != *(unsigned short *)(rec + 0xb)
             || *(short *)(h + 8) != *(unsigned short *)(rec + 0xd))
                param_1[3] = 8;
        }
    }
}
