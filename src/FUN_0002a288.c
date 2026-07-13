/* FUN_0002a288 @ 0x2a288 -- radar/status panel state machine.
 * TRUE SIZE 1427 (0x593, ret at 0x2a81a; manifest said 304 -- headless sweep
 * truncated at the first indirect jmp). TWO jump tables decoded via lefix.py:
 *  - outer 6-entry at literal 0x1cad8 (manifest 0x2a220): state-1 -> case
 *    1/2: 0x2a3ec, 3: 0x2a816 (default/end), 4: 0x2a40e, 5/6: 0x2a7f8.
 *  - inner 20-entry at literal 0x1caf0 (manifest 0x2a238): item type ->
 *    0: 0x2a75a (skip), 1..0xc: 0x2a454+0x2f*k (per-type draw), 0xd/e/f:
 *    0x2a6ca, 0x10: 0x2a6f6, 0x11/12/13: 0x2a722; >0x13 -> 0x2a75a.
 *
 * Semantics: clears g_52ff, state=1. If word g_5390 >= 0x80: index the
 * 0x417-byte mission records at 0xe551 by (short)g_10b16; pool-A record
 * p = 0x8110 + 0x5c*((uchar)rec[0] + (schar)rec[1]); state=2. If entity id
 * word g_10b14 is live (nonzero, its record (id/0x5c)>>3 != g_10b16, dead
 * flag [+0xb]&1 clear): target id p[0x44] -> 0 => state 5 else LOS check
 * FUN_0002e5f8(p, ent, g_a6c2[weapon type]) => state 6/5. Else if word
 * g_10b12 live: same via FUN_0002e808 => 6/5. Else if g_10b1a: state 4,
 * n = FUN_000377b8(p) (carried-item count). Dispatch switch(state):
 *  1/2 -> g_5324 = g_5308 + 6*state; return.   3 -> nothing.
 *  4   -> g_52ff=1; e = g_810e + g_10b1a; if g_e398==0 and n<8: inner
 *         switch on type e[0x19] queues a draw-list node
 *         FUN_0001b858(*(ushort*)(g_5348+OFS), g_5258[n*9], g_5258[n*9+1]),
 *         OFS = 0x242+8*(type-1) for 1..0xc, 0x2b2 for 0xd/e/f, 0x2ba for
 *         0x10, 0x2c2 for 0x11/12/13; type 0xc also centers string
 *         tbl_4b10[type-1][g_a50d] into field 0x10584; then if g_a69a[type]
 *         draw gauge FUN_0003fb40(x+4, y+0x18, 0x17, 4, 0xc). Always:
 *         center tbl_4b10[type-1][g_a50d] into 0x10584; g_5324 = g_5308 +
 *         6*state; return.
 *  5/6 -> g_5324 = g_5308 + 6*state; fall out.
 *
 * PARKED near-miss (NOT matched): fair masked-aligned 1182/1427 (~83%),
 * length 1419 vs 1427. Everything structural matches: both jump tables,
 * the inner switch cross-jumps its 14 call tails into ONE merged
 * and/push/call/add block (arms must all exit via break -- see 0x29ad8
 * notes), per-case index temp m=n*9 stops the 9n recompute, 2D decl
 * tbl_4b10[][3] gives both two-term SIB lookups. Correct coloring
 * (state=ESI, p=ECX, e1/e=EDI, n=EBX) REQUIRES the if/else-if chain
 * spelling for the state 5/6 blocks; the &&-form, ||-form, goto-into-else
 * and state-as-tid spellings all flip state->EDI and rotate p/e1 (a
 * coloring/layout DEADLOCK: the chain form places state=5 before the
 * call block, the target wants 6-first with one shared st5 -- no spelling
 * gives both). Residues: that layout inversion; entry loads g_5390 via DX
 * vs direct cmp mem,imm (+2B); e1 id built di-load form vs target
 * xor-eax + mov edi,0x810e + add (uint id1 fixes the form but rotates
 * p->EDI); div quotient EDX vs EBP; case-body y-reg EDX vs ECX and
 * movsx bx copy alternating ecx/edx vs uniform ecx; case-c/send-final
 * 4b10 operand roles rotated. Recipe: -4s -oneatx -zp8 -s -zq. */
extern unsigned char g_52ff;
extern unsigned short g_5390;
extern short g_10b16;
extern unsigned short g_10b14;
extern unsigned short g_10b12;
extern unsigned short g_10b1a;
extern unsigned char g_e551[];
extern unsigned char g_8110[];
extern unsigned char g_810e[];
extern short g_a6c2[];
extern unsigned char g_e398;
extern unsigned char *g_5348;
extern short g_5258[];
extern unsigned char g_a69a[];
extern int tbl_4b10[][3];
extern unsigned char g_a50d;
extern int g_5308;
extern int g_5324;
extern unsigned char *FUN_0002e5f8(unsigned char *a, unsigned char *rec, short p);
extern unsigned char *FUN_0002e808(unsigned char *p1, unsigned char *p2, int dist);
extern int FUN_000377b8(unsigned char *p);
extern void FUN_0001b858(unsigned short a, short x, short y);
extern int FUN_0003fb40();
extern void FUN_000299c8(int a, int b);

void FUN_0002a288(void)
{
    unsigned short state;
    short n;
    unsigned char *p;
    unsigned char *e1;
    unsigned char *e2;
    unsigned char *e;
    unsigned short id1;
    unsigned short id2;
    unsigned short dx;
    int m;
    unsigned short tid;
    int k;

    g_52ff = 0;
    state = 1;
    if (g_5390 >= 0x80) {
        k = g_10b16 * 0x417;
        p = g_8110 + ((signed char)g_e551[k + 1] + g_e551[k]) * 0x5c;
        id1 = g_10b14;
        e1 = g_810e + id1;
        state = 2;
        if (id1 != 0
            && ((unsigned int)id1 / 0x5c) >> 3 != g_10b16
            && (e1[0xb] & 1) == 0) {
            tid = *(unsigned short *)(p + 0x44);
            if (tid == 0) {
                state = 5;
            } else if (FUN_0002e5f8(p, e1, g_a6c2[g_810e[tid + 0x19]])) {
                state = 6;
            } else {
                state = 5;
            }
        } else {
            id2 = g_10b12;
            if (id2 != 0) {
                e2 = g_810e + id2;
                if ((e2[0xb] & 1) == 0) {
                    dx = *(unsigned short *)(p + 0x44);
                    if (dx == 0) {
                        state = 5;
                    } else if (FUN_0002e808(p, e2, g_a6c2[g_810e[dx + 0x19]])) {
                        state = 6;
                    } else {
                        state = 5;
                    }
                }
            } else if (g_10b1a != 0) {
                state = 4;
                n = FUN_000377b8(p);
            }
        }
    }
    switch (state) {
    case 1:
    case 2:
        g_5324 = g_5308 + state * 6;
        return;
    case 3:
        break;
    case 4:
        g_52ff = 1;
        e = g_810e + g_10b1a;
        if (g_e398 == 0 && n < 8) {
            switch (e[0x19]) {
            case 1:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x242), g_5258[m], g_5258[m + 1]);
                break;
            case 2:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x24a), g_5258[m], g_5258[m + 1]);
                break;
            case 3:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x252), g_5258[m], g_5258[m + 1]);
                break;
            case 4:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x25a), g_5258[m], g_5258[m + 1]);
                break;
            case 5:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x262), g_5258[m], g_5258[m + 1]);
                break;
            case 6:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x26a), g_5258[m], g_5258[m + 1]);
                break;
            case 7:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x272), g_5258[m], g_5258[m + 1]);
                break;
            case 8:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x27a), g_5258[m], g_5258[m + 1]);
                break;
            case 9:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x282), g_5258[m], g_5258[m + 1]);
                break;
            case 0xa:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x28a), g_5258[m], g_5258[m + 1]);
                break;
            case 0xb:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x292), g_5258[m], g_5258[m + 1]);
                break;
            case 0xc:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x29a), g_5258[m], g_5258[m + 1]);
                FUN_000299c8(0x10584, tbl_4b10[e[0x19] - 1][g_a50d]);
                break;
            case 0xd:
            case 0xe:
            case 0xf:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x2b2), g_5258[m], g_5258[m + 1]);
                break;
            case 0x10:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x2ba), g_5258[m], g_5258[m + 1]);
                break;
            case 0x11:
            case 0x12:
            case 0x13:
                m = n * 9;
                FUN_0001b858(*(unsigned short *)(g_5348 + 0x2c2), g_5258[m], g_5258[m + 1]);
                break;
            case 0:
            default:
                break;
            }
            if (g_a69a[e[0x19]] != 0)
                FUN_0003fb40((unsigned short)g_5258[n * 9] + 4,
                             (unsigned short)g_5258[n * 9 + 1] + 0x18, 0x17, 4, 0xc);
        }
        FUN_000299c8(0x10584, tbl_4b10[e[0x19] - 1][g_a50d]);
        g_5324 = g_5308 + state * 6;
        return;
    case 5:
    case 6:
        g_5324 = g_5308 + state * 6;
        break;
    }
}
