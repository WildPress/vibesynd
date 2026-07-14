/* FUN_00021658 @ 0x21658 -- "new-game / player reset": recompute equipment &
 * research availability flags for the current player, and (when a research
 * tier is NOT yet owned) reset that player's funds to 100,000,000 and rebuild
 * the squad-order / conveyor / equipment-template tables from defaults.
 *
 * TRUE SIZE = 1969 bytes (0x21658..0x21e08 inclusive, ending at its RET).
 * The manifest's 3424 is WRONG: the headless sweep merged the ADJACENT but
 * SEPARATE function at 0x21e18 (its own push ebx/esi/edi/ebp prologue, takes a
 * param, ends 0x223b7 -> the squad re-equip routine, cf. sibling 0x223c8) plus
 * 15 bytes of inter-function LEA-NOP padding: 1969 + 15 + 1440 = 3424. Nothing
 * in the first function jumps to 0x21e18; all exits funnel to the RET at
 * 0x21e08. Score with:  truediff.py FUN_00021658 1969
 *
 * The current player's economy/template record is  rec = g_e49c + 0x417*g_10b16
 * (stride 1047, indexed by the current-player short g_10b16 -- the 0x417 family,
 * cf. siblings 0x223c8 / 0x12da8 / 0x23158). rec+0x11 and rec+0x23 are two
 * 0x1a0-bit "owned-research" bitsets; FUN_0003aee6(bitset, id) tests bit `id`.
 * Six research tiers are probed by id (0x1a0,0x1ac,0x1bc,0x1c8,0x1d4,0x1e0),
 * each requiring the bit set in BOTH bitsets to count as owned:
 *   tier 0x1a0 -> g_539a   (not-owned => 1)
 *   tier 0x1ac -> g_10b52
 *   tier 0x1bc -> g_10b48
 *   tier 0x1c8 -> g_10b43, and reset funds rec[0] = 100000000
 *   tier 0x1d4 -> g_539a & g_10b43, funds, + rebuild squad/conveyor tables
 *   tier 0x1e0 -> g_539a & g_10b43, funds, + rebuild tables + equip templates
 * (The last two tiers gate the heavy default-loadout rebuild.)
 *
 * Rebuild step 1 (squad grids): 18 records of 491 bytes based at g_5788:
 *   g_5788[d*491] = 2400 (a capacity), and the 10x24 word grid at g_578a
 *   (d*491 + row*48 + col*2) is zeroed.
 * Rebuild step 2 (conveyor rows): 20 records of 501 bytes based at g_7bf4:
 *   skip any row whose type byte g_7bf4[e*501] == 0xfe; else g_7c05[e*501]=2400,
 *   zero the 10x24 word grid at g_7c07, copy g_7de8[e*501] = g_b830[e], and
 *   take the absolute value of the signed dword g_7bf5[e*501].
 * Rebuild step 3 (equip templates): for each of 18 entries (stride 40) inside
 *   rec, roll a byte g_e5b9 = FUN_20c88(), set the HP word g_e5ba = 0x10, the
 *   flags word g_e5bc = 0x1fff, and eight {qty=g_a73a[kind], kind} item slots
 *   at g_e5c1/g_e5c3 for the fixed kind list {6,6,1,0xc,0x11,0x11,7,7}.
 *
 * STATUS: full readable-C decode, PARKED. Score vs TRUE size 1969:
 *   -4s -oneatx -zp8 -s -zq  ->  obj 1886B (delta -83B), first diff @0x2ab.
 * The whole probe/flag half (blocks A-D + the block-E funds write, 683 bytes)
 * is BYTE-EXACT. The three loop-carried locals had to be forced memory-homed to
 * reach the target's `sub esp,0xc` frame + slot layout: the block-E conveyor
 * counter -> [esp+4], the block-F one -> [esp], and the block-G roll -> [esp+8]
 * (the `volatile int e1,e2,roll` lever; without it -oneatx registerises them and
 * the frame/length collapse to 1770B). The residual -83B and all remaining diffs
 * sit inside the two nested fill loops and are pure MATCHING-PLAYBOOK section-3
 * register-role / accumulator-selection ties, NOT missing C:
 *   - di*491 materialise: target `xor ecx,ecx; mov cx,di; imul ecx,ebp` (index
 *     straight into the ECX accumulator) vs ours `xor eax,eax; mov ax,di;
 *     mov ecx,eax; imul ecx,ebp` (index to EAX, copied to ECX);
 *   - inner sum: target `add ecx,eax` (accumulate in ECX) vs ours
 *     `lea edx,[ecx+eax]` (result to EDX) -- accumulator-selection tie (class 2);
 *   - the stored zero: target `xor edx,esi` (reuse the known-equal SI) vs ours
 *     `xor ecx,ecx`.
 * Every C spelling + the register-role wall converge here; parked per section 3.
 * The @0x2ab diff itself is only a forward-JNZ rel32 that cascades from the -83B
 * tail, not a structural divergence.
 */

extern short         g_10b16;
extern unsigned char g_539a;
extern unsigned char g_10b52;
extern unsigned char g_10b43;
extern unsigned char g_10b48;

extern unsigned char g_e49c[];   /* economy/template record region (stride 0x417) */
extern unsigned char g_5788[];   /* squad-grid record capacity word  (stride 491) */
extern unsigned char g_578a[];   /* squad-grid 10x24 word body */
extern unsigned char g_7bf4[];   /* conveyor-row type byte           (stride 501) */
extern unsigned char g_7bf5[];   /* conveyor-row signed dword */
extern unsigned char g_7c05[];   /* conveyor-row capacity word */
extern unsigned char g_7c07[];   /* conveyor-row 10x24 word body */
extern unsigned char g_7de8[];   /* conveyor-row source byte */
extern unsigned char g_b830[];   /* per-row default source table */
extern unsigned char g_e5b9[];   /* equip-template roll byte (in rec, stride 40) */
extern unsigned char g_e5ba[];   /* equip-template HP word */
extern unsigned char g_e5bc[];   /* equip-template flags word */
extern unsigned char g_e5c1[];   /* equip-template slot qty word */
extern unsigned char g_e5c3[];   /* equip-template slot kind word */
extern short         g_a73a[];   /* item kind -> default quantity table */

extern int  FUN_0003aee6(unsigned char *bitset, int id);
extern int  FUN_00020c88(void);

void FUN_00021658(void)
{
    volatile int e1, e2;
    volatile int roll;
    unsigned short d, row, col;

    g_539a  = 0;
    g_10b52 = 0;
    g_10b43 = 0;
    g_10b48 = 0;

    /* --- block A: research tier 0x1a0 --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1a0) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1a0) == 0)
        g_539a = 1;

    /* --- block B: research tier 0x1ac --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1ac) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1ac) == 0)
        g_10b52 = 1;

    /* --- block C: research tier 0x1bc --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1bc) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1bc) == 0)
        g_10b48 = 1;

    /* --- block D: research tier 0x1c8 -> also reset funds --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1c8) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1c8) == 0) {
        g_10b43 = 1;
        *(int *)(g_e49c + 0x417 * g_10b16) = 100000000;
    }

    /* --- block E: research tier 0x1d4 -> funds + rebuild squad/conveyor --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1d4) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1d4) == 0) {
        g_539a  = 1;
        g_10b43 = 1;
        *(int *)(g_e49c + 0x417 * g_10b16) = 100000000;

        for (d = 0; d < 0x12; d++) {
            *(short *)(g_5788 + d * 491) = 2400;
            for (row = 0; row < 0xa; row++)
                for (col = 0; col < 0x18; col++)
                    *(short *)(g_578a + d * 491 + row * 48 + col * 2) = 0;
        }
        for (e1 = 0; e1 < 0x14; e1++) {
            if (g_7bf4[e1 * 501] == 0xfe)
                continue;
            *(short *)(g_7c05 + e1 * 501) = 2400;
            for (row = 0; row < 0xa; row++)
                for (col = 0; col < 0x18; col++)
                    *(short *)(g_7c07 + e1 * 501 + row * 48 + col * 2) = 0;
            g_7de8[e1 * 501] = g_b830[e1];
            if (*(int *)(g_7bf5 + e1 * 501) < 0)
                *(int *)(g_7bf5 + e1 * 501) = -*(int *)(g_7bf5 + e1 * 501);
        }
    }

    /* --- block F: research tier 0x1e0 -> rebuild + equip templates --- */
    if (FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x23, 0x1e0) == 0 ||
        FUN_0003aee6(g_e49c + 0x417 * g_10b16 + 0x11, 0x1e0) == 0) {
        g_539a  = 1;
        g_10b43 = 1;
        *(int *)(g_e49c + 0x417 * g_10b16) = 100000000;

        for (d = 0; d < 0x12; d++) {
            *(short *)(g_5788 + d * 491) = 2400;
            for (row = 0; row < 0xa; row++)
                for (col = 0; col < 0x18; col++)
                    *(short *)(g_578a + d * 491 + row * 48 + col * 2) = 0;
        }
        for (e2 = 0; e2 < 0x14; e2++) {
            if (g_7bf4[e2 * 501] == 0xfe)
                continue;
            *(short *)(g_7c05 + e2 * 501) = 2400;
            for (row = 0; row < 0xa; row++)
                for (col = 0; col < 0x18; col++)
                    *(short *)(g_7c07 + e2 * 501 + row * 48 + col * 2) = 0;
            g_7de8[e2 * 501] = g_b830[e2];
            if (*(int *)(g_7bf5 + e2 * 501) < 0)
                *(int *)(g_7bf5 + e2 * 501) = -*(int *)(g_7bf5 + e2 * 501);
        }

        /* equip templates: 18 entries of stride 40 inside rec; the eight item
         * slots (kinds 6,6,1,0xc,0x11,0x11,7,7) are fully unrolled */
        for (d = 0; d < 0x12; d++) {
            roll = FUN_00020c88();
            g_e5b9[0x417 * g_10b16 + d * 40] = (unsigned char)roll;
            *(short *)(g_e5ba + 0x417 * g_10b16 + d * 40) = 0x10;
            *(short *)(g_e5bc + 0x417 * g_10b16 + d * 40) = 0x1fff;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x00) = g_a73a[6];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x00) = 6;
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x04) = 6;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x04) = g_a73a[6];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x08) = 1;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x08) = g_a73a[1];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x0c) = 0xc;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x0c) = g_a73a[0xc];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x10) = 0x11;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x10) = g_a73a[0x11];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x14) = 0x11;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x14) = g_a73a[0x11];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x18) = 7;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x18) = g_a73a[7];
            *(short *)(g_e5c3 + 0x417 * g_10b16 + d * 40 + 0x1c) = 7;
            *(short *)(g_e5c1 + 0x417 * g_10b16 + d * 40 + 0x1c) = g_a73a[7];
        }
    }
}
