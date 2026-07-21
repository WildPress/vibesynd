/* 0x34608 -- pick a passable direction for the current projectile/scan point.
 * If dir is vertical (0 / 0x80) and the point's x tile differs from the target
 * x tile (g_target_x), first try the direction vec_to_angle(dx,0) computed from
 * the tile delta; then try dir itself, dir-0x40, dir+0x40, each tested with
 * the 4-way tile lookup compass_tile_probe(x=g_shot_x, y=g_shot_y, z=g_shot_level, d).
 * Horizontal dirs (0xc0 / 0x40) mirror with y tiles (g_shot_y vs g_target_y) and
 * vec_to_angle(0,dy). Returns the chosen direction (dir unchanged if nothing
 * passes). Recipe: -4s -oneatx -zp8 -s -zq
 *
 * NEAR-MATCH, 573/590, EDIT-DIST 198 (was 206). FIX: the tile-delta compare
 * is written global-on-the-minuend-side, `g_target_x >> 8 != g_shot_x >> 8` (and
 * the horizontal `g_target_y >> 8 != g_shot_y >> 8`). This parks g_target_x in EAX so
 * `d = (g_target_x>>8)-(g_shot_x>>8)` becomes `SUB EAX,EDX; CWDE` matching target,
 * instead of the old `SUB EDX,EAX; MOVSX EAX,DX` (+2B). Register CONTENTS of the
 * load block now match (g_shot_x=EDX, g_target_x=EAX); only the load ORDER and the
 * `cmp` operand order stay swapped (a fwd-lookahead alloc tie, not C-reachable).
 * Two residual walls remain: (1) the dir+-0x40 returns: target masks to
 * (unsigned char) via `xor ah,ah` (so dir=0 gives 0x00C0), but writing that cast
 * makes OUR Watcom cross-jump-MERGE the four byte-identical return blocks that
 * target keeps INLINED -> worse (221). The current (unsigned short)(t+-0x40)
 * form avoids the bad merge at the cost of the missing byte-mask; it scores
 * better, so it stays. (2) CROSS-JUMP ASYMMETRY on the `return dir` tails: target
 * shares block-1's return-dir with the final tail yet inlines block-2's identical
 * one; ours merges both. Both walls are Watcom single-pass cross-jump decisions
 * (exit-form driven) not reachable from C. Weapons-cluster poor-exact-match class.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_target_x;
extern short g_target_y;
extern short g_shot_level;
extern short g_shot_y;
extern short g_shot_x;
extern unsigned short vec_to_angle(int dx, int dy);
extern int compass_tile_probe(int x, int y, int z, int dir);

unsigned short pick_passable_shot_dir(unsigned char dir)
{
    volatile unsigned short nd2;
    volatile unsigned short nd1;

    if (dir == 0 || dir == 0x80) {
        if (g_target_x >> 8 != g_shot_x >> 8) {
            short d = (g_target_x >> 8) - (g_shot_x >> 8);
            nd1 = vec_to_angle(d, 0);
            if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)nd1) != 0)
                return (unsigned char)nd1;
        }
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, dir) != 0)
            return dir;
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)(dir - 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t - 0x40);
        }
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)(dir + 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t + 0x40);
        }
    } else if (dir == 0xc0 || dir == 0x40) {
        if (g_target_y >> 8 != g_shot_y >> 8) {
            short d = (g_target_y >> 8) - (g_shot_y >> 8);
            nd2 = vec_to_angle(0, d);
            if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)nd2) != 0)
                return (unsigned char)nd2;
        }
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, dir) != 0)
            return dir;
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)(dir - 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t - 0x40);
        }
        if ((short)compass_tile_probe(g_shot_x, g_shot_y, g_shot_level, (unsigned char)(dir + 0x40)) != 0) {
            unsigned int t = dir;
            return (unsigned short)(t + 0x40);
        }
    }
    return dir;
}
