/* @ 0x34858: top-level weapon-fire routine (964B, 8 distinct callees). Two
 * top-level modes on the firing entity's type byte p2[0x19]:
 *  - type 5/6 (guided/tracking): step the shot cursor (g_aim_x/g_aim_y) one
 *    tile toward the shooter's target coords, pick facing via FUN_0004d221,
 *    snap the off-axis coord toward centre via FUN_00034048, adjust the charge
 *    byte p1[0x54] by the remaining tile distance, drop the shot if it reached
 *    the tile (FUN_0002d998), accumulate the shot vector (FUN_00026ad8) and
 *    commit the cursor (FUN_00026c78).
 *  - otherwise: publish the shot accumulators g_shot_x/g_shot_y/g_shot_level and the
 *    target coords g_10b54/56/58, run the trajectory march FUN_00034198, snap
 *    the cursor to the facing (FUN_00034048 by octant), re-pick the passable
 *    facing (FUN_00034608) when the cursor moved off the target tile, probe the
 *    blocked-tile map (g_map_cols column table -> g_tile_flags class) and drop on a block,
 *    then accumulate + commit as above.
 * Recipe: -4s -oneatx -zp8 -s -zq
 *
 * PARKED near-miss ~74-77% (963B vs 964B). SEMANTICS BYTE-CORRECT: the prologue
 * (31B) is byte-identical, the type-5/6 mode's whole call/arith sequence is
 * byte-correct bar the spill below, and the "otherwise" mode is byte-IDENTICAL
 * through offset 0x54 (the FUN_00034198 setup+call) with the g_map_cols blocked-tile
 * lookup structurally exact (in-place `add`, correct idiv/%0x6000, /256, *128,
 * /128 tile math). Multiple INDEPENDENT register-role / encoding walls (playbook
 * s3), no one of which is source-reachable:
 *   1. type-5/6 dxa/dya spill (block-A first diff 0x1b). Target keeps dxa (1st
 *      labs) in EDI and slot-homes dya, so the x-branch ends `mov eax,edi` (2B);
 *      the `(short)` cast needed for the 16-bit `cmp di,word[esp]` forces -oneatx
 *      to spill dxa to the slot and reload it (`mov eax,[esp]`, 3B) + eager pop.
 *      Without the cast dxa DOES stay in EDI, but the compare becomes 32-bit.
 *      volatile-dya / &dya / register / decl-order / operand-swap / -or/-ot/-oa/
 *      -oc all fail (each flips one byte the other way or diverges elsewhere).
 *   2. facing switch (block-B first structural diff 0x54): target dispatches with
 *      a NEAR `0f82` jc where our -oneatx emits the rel8 `72` that fits (a Watcom
 *      jump-encoding peephole, same class as the push imm8/imm32 wall); case-body
 *      reorder to the target's physical order fixes the dispatch but breaks the
 *      case-0/0x80 g_aim_x cross-jump (+5B), so the two are mutually exclusive.
 *   3. g_map_cols column lookup: byte-structurally identical to target AFTER the
 *      `base += index` in-place lever (gives `add edi,eax` not `lea`), residual is
 *      a consistent EDI<->ECX register-role swap of the divisor/row/base triangle
 *      -- the exact FUN_0002d5b8 / FUN_00033e78 g_map_cols column register wall.
 *   4. tile-class widen: target `xor edx,edx; mov dl,[eax]` vs ours `mov al;
 *      and eax,0xff` (uchar and-form vs xor-first; FUN_0002d5b8 wall).
 */
extern short g_aim_x;
extern short g_aim_y;
extern short g_10b2c;
extern short g_shot_x;
extern short g_shot_y;
extern short g_shot_level;
extern short g_10b54;
extern short g_10b56;
extern short g_10b58;
extern char **g_map_cols;
extern unsigned char *g_tile_flags;

extern int FUN_0003aed8(int x);
extern unsigned char FUN_0004d221(int dx, int dy);
extern short FUN_00034048(int cur, int step);
extern void FUN_0002d998(unsigned char *p);
extern void FUN_00026ad8(unsigned short mult, unsigned short idx);
extern void FUN_00034198(unsigned char *p2, unsigned char *p, unsigned short count);
extern unsigned short FUN_00034608(int dir);
extern void FUN_00026c78(unsigned char *node, int x, int y, int z);

void FUN_00034858(unsigned char *p1, unsigned char *p2)
{
    int z;

    if (p2[0x19] == 5 || p2[0x19] == 6) {
        int dxa = FUN_0003aed8((*(short *)(p1 + 0x2e) >> 8) - (g_aim_x >> 8));
        int dya = FUN_0003aed8((*(short *)(p1 + 0x30) >> 8) - (g_aim_y >> 8));
        unsigned short steps;

        if ((short)dxa > (short)dya) {
            p2[0x29] = FUN_0004d221((short)(*(short *)(p1 + 0x2e) - g_aim_x), 0);
            g_aim_y = FUN_00034048(g_aim_y, 0x80);
            steps = dxa;
        } else {
            p2[0x29] = FUN_0004d221(0, (short)(*(short *)(p1 + 0x30) - g_aim_y));
            g_aim_x = FUN_00034048(g_aim_x, 0x80);
            steps = dya;
        }

        switch (steps) {
        case 0:
            p1[0x54] = 0;
            break;
        case 1:
        case 2:
            p1[0x54] -= 0xc;
            if (p1[0x54] < 0xc)
                p1[0x54] = 0xc;
            break;
        case 3:
            p1[0x54] = p1[0x55] / 2;
            break;
        default:
            p1[0x54] += 4;
            if (p1[0x54] > p1[0x55])
                p1[0x54] = p1[0x55];
            break;
        }

        if (g_aim_x >> 8 == *(short *)(p1 + 0x2e) >> 8
            && g_aim_y >> 8 == *(short *)(p1 + 0x30) >> 8)
            FUN_0002d998(p1);

        FUN_00026ad8((unsigned short)p1[0x54], (unsigned short)p2[0x29]);
        z = *(short *)(p2 + 8);
    } else {
        g_shot_x = g_aim_x;
        g_shot_y = g_aim_y;
        g_shot_level = g_10b2c;
        g_10b54 = *(short *)(p1 + 0x2e);
        g_10b56 = *(short *)(p1 + 0x30);
        g_10b58 = *(short *)(p1 + 0x32);
        FUN_00034198(p2, p1, 6);

        switch (p2[0x1a]) {
        case 0x00:
            g_aim_x = FUN_00034048(g_aim_x, 0xc0);
            break;
        case 0x40:
            g_aim_y = FUN_00034048(g_aim_y, 0x40);
            break;
        case 0x80:
            g_aim_x = FUN_00034048(g_aim_x, 0x40);
            break;
        case 0xc0:
            g_aim_y = FUN_00034048(g_aim_y, 0xc0);
            break;
        }

        if (g_aim_x >> 8 != *(short *)(p1 + 0x34) >> 8
            || g_aim_y >> 8 != *(short *)(p1 + 0x36) >> 8) {
            g_shot_x = g_aim_x;
            g_shot_y = g_aim_y;
            g_shot_level = g_10b2c;
            p2[0x1a] = (unsigned char)FUN_00034608(p2[0x1a]);
            *(short *)(p1 + 0x34) = g_aim_x;
            *(short *)(p1 + 0x36) = g_aim_y;
        }

        if (!(p1[0x1c] & 2) && (p2[0xd] & 2)) {
            p1[0x54] = 0;
            p2[0xd] &= ~2;
        }

        if (g_aim_x >> 8 == *(short *)(p1 + 0x2e) >> 8
            && g_aim_y >> 8 == *(short *)(p1 + 0x30) >> 8) {
            int row = (g_aim_y % 0x6000) / 256;
            int col = (g_aim_x & 0xff00) / 256;
            char **base = g_map_cols;
            int index = col + row * 128;
            base += index;
            if (g_tile_flags[*(unsigned char *)((g_10b2c - 1) / 128 + (int)*base)] != 0)
                FUN_0002d998(p1);
        }

        if (p1[0x54] > 0)
            FUN_00026ad8((unsigned short)p1[0x54], (unsigned short)p2[0x1a]);
        z = g_10b2c;
    }

    FUN_00026c78(p2, g_aim_x, g_aim_y, z);
}
