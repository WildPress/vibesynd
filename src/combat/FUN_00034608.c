/* 0x34608 -- pick a passable direction for the current projectile/scan point.
 * If dir is vertical (0 / 0x80) and the point's x tile differs from the target
 * x tile (g_10b54), first try the direction FUN_0004d221(dx,0) computed from
 * the tile delta; then try dir itself, dir-0x40, dir+0x40, each tested with
 * the 4-way tile lookup FUN_00034368(x=g_10b5e, y=g_10b5c, z=g_10b5a, d).
 * Horizontal dirs (0xc0 / 0x40) mirror with y tiles (g_10b5c vs g_10b56) and
 * FUN_0004d221(0,dy). Returns the chosen direction (dir unchanged if nothing
 * passes). Recipe: -4s -oneatx -zp8 -s -zq
 *
 * PARKED near-miss, 577/590 (13 bytes short). Semantics byte-correct; two
 * register/layout walls: (1) the `short d` subtraction homes in AX in the
 * target (`SUB EAX,EDX; CWDE`) but DX in ours (`MOVSX EAX,DX`); (2) CROSS-JUMP
 * ASYMMETRY on the `return dir` tails — the target SHARES block-1's return-dir
 * with the function's final `xor eax; mov al,bl` tail (`xor eax; jmp end`) yet
 * INLINES block-2's identical return-dir, from the SAME `return dir;` source.
 * Ours merges BOTH into one shared tail, 13 bytes shorter. Watcom's single-pass
 * cross-jumper makes physically-driven merge decisions not reachable from C
 * (cf. the cont.22 cross-jump law: exit-form controls merging, but here the two
 * blocks need OPPOSITE decisions). Weapons-cluster poor-exact-match class.
 */
extern short g_10b54;
extern short g_10b56;
extern short g_10b5a;
extern short g_10b5c;
extern short g_10b5e;
extern unsigned short FUN_0004d221(int dx, int dy);
extern int FUN_00034368(int x, int y, int z, int dir);

unsigned short FUN_00034608(unsigned char dir)
{
    volatile unsigned short nd2;
    volatile unsigned short nd1;

    if (dir == 0 || dir == 0x80) {
        if (g_10b5e >> 8 != g_10b54 >> 8) {
            short d = (g_10b54 >> 8) - (g_10b5e >> 8);
            nd1 = FUN_0004d221(d, 0);
            if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)nd1) != 0)
                return (unsigned char)nd1;
        }
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, dir) != 0)
            return dir;
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)(dir - 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t - 0x40);
        }
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)(dir + 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t + 0x40);
        }
    } else if (dir == 0xc0 || dir == 0x40) {
        if (g_10b5c >> 8 != g_10b56 >> 8) {
            short d = (g_10b56 >> 8) - (g_10b5c >> 8);
            nd2 = FUN_0004d221(0, d);
            if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)nd2) != 0)
                return (unsigned char)nd2;
        }
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, dir) != 0)
            return dir;
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)(dir - 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t - 0x40);
        }
        if ((short)FUN_00034368(g_10b5e, g_10b5c, g_10b5a, (unsigned char)(dir + 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t + 0x40);
        }
    }
    return dir;
}
