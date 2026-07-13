/* @ 0x34858: top-level weapon-fire routine (964B, 8 distinct callees). Two
 * top-level modes on the firing entity's type byte p2[0x19]:
 *  - type 5/6 (guided/tracking): step the shot cursor (g_10b28/g_10b2a) one
 *    tile toward the shooter's target coords, pick facing via FUN_0004d221,
 *    snap the off-axis coord toward centre via FUN_00034048, adjust the charge
 *    byte p1[0x54] by the remaining tile distance, drop the shot if it reached
 *    the tile (FUN_0002d998), accumulate the shot vector (FUN_00026ad8) and
 *    commit the cursor (FUN_00026c78).
 *  - otherwise: publish the shot accumulators g_10b5e/g_10b5c/g_10b5a and the
 *    target coords g_10b54/56/58, run the trajectory march FUN_00034198, snap
 *    the cursor to the facing (FUN_00034048 by octant), re-pick the passable
 *    facing (FUN_00034608) when the cursor moved off the target tile, probe the
 *    blocked-tile map (g_5358 column table -> g_10ac0 class) and drop on a block,
 *    then accumulate + commit as above.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_10b28;
extern short g_10b2a;
extern short g_10b2c;
extern short g_10b5e;
extern short g_10b5c;
extern short g_10b5a;
extern short g_10b54;
extern short g_10b56;
extern short g_10b58;
extern char **g_5358;
extern unsigned char *g_10ac0;

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
        int dxa = FUN_0003aed8((*(short *)(p1 + 0x2e) >> 8) - (g_10b28 >> 8));
        int dya = FUN_0003aed8((*(short *)(p1 + 0x30) >> 8) - (g_10b2a >> 8));
        int *pdya = &dya;
        unsigned short steps;

        if ((short)dxa > (short)*pdya) {
            p2[0x29] = FUN_0004d221((short)(*(short *)(p1 + 0x2e) - g_10b28), 0);
            g_10b2a = FUN_00034048(g_10b2a, 0x80);
            steps = dxa;
        } else {
            p2[0x29] = FUN_0004d221(0, (short)(*(short *)(p1 + 0x30) - g_10b2a));
            g_10b28 = FUN_00034048(g_10b28, 0x80);
            steps = *pdya;
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

        if (g_10b28 >> 8 == *(short *)(p1 + 0x2e) >> 8
            && g_10b2a >> 8 == *(short *)(p1 + 0x30) >> 8)
            FUN_0002d998(p1);

        FUN_00026ad8((unsigned short)p1[0x54], (unsigned short)p2[0x29]);
        z = *(short *)(p2 + 8);
    } else {
        g_10b5e = g_10b28;
        g_10b5c = g_10b2a;
        g_10b5a = g_10b2c;
        g_10b54 = *(short *)(p1 + 0x2e);
        g_10b56 = *(short *)(p1 + 0x30);
        g_10b58 = *(short *)(p1 + 0x32);
        FUN_00034198(p2, p1, 6);

        switch (p2[0x1a]) {
        case 0x00:
            g_10b28 = FUN_00034048(g_10b28, 0xc0);
            break;
        case 0x40:
            g_10b2a = FUN_00034048(g_10b2a, 0x40);
            break;
        case 0x80:
            g_10b28 = FUN_00034048(g_10b28, 0x40);
            break;
        case 0xc0:
            g_10b2a = FUN_00034048(g_10b2a, 0xc0);
            break;
        }

        if (g_10b28 >> 8 != *(short *)(p1 + 0x34) >> 8
            || g_10b2a >> 8 != *(short *)(p1 + 0x36) >> 8) {
            g_10b5e = g_10b28;
            g_10b5c = g_10b2a;
            g_10b5a = g_10b2c;
            p2[0x1a] = (unsigned char)FUN_00034608(p2[0x1a]);
            *(short *)(p1 + 0x34) = g_10b28;
            *(short *)(p1 + 0x36) = g_10b2a;
        }

        if (!(p1[0x1c] & 2) && (p2[0xd] & 2)) {
            p1[0x54] = 0;
            p2[0xd] &= ~2;
        }

        if (g_10b28 >> 8 == *(short *)(p1 + 0x2e) >> 8
            && g_10b2a >> 8 == *(short *)(p1 + 0x30) >> 8) {
            int row = (g_10b2a % 0x6000) / 256;
            int col = (g_10b28 & 0xff00) / 256;
            char **base = g_5358;
            int index = col + row * 128;
            char **slot = base + index;
            unsigned char tile = *(unsigned char *)((g_10b2c - 1) / 128 + (int)*slot);
            if (g_10ac0[tile] != 0)
                FUN_0002d998(p1);
        }

        if (p1[0x54] > 0)
            FUN_00026ad8((unsigned short)p1[0x54], (unsigned short)p2[0x1a]);
        z = g_10b2c;
    }

    FUN_00026c78(p2, g_10b28, g_10b2a, z);
}
