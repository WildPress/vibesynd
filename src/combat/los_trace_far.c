/* frameless @ 0x2e808: line-of-sight trace from entity p1 toward entity p2.
   d1 = dist(p2-p1) via FUN_14c58; if d1 >= range, fail. steps = range/0x80
   (must be >= 1). dir1 = angle(p2-p1 in xy) via FUN_4d221, d2 = xy distance,
   dir2 = angle(dz, d2) (vertical). March x/y/z from p1's coords in half-tile
   increments (g_dir_dx/g_dir_dy direction tables * 0x80 >> 8): each step must be
   passable (FUN_f898(x,y,z+0x80) == 0); FUN_11d68(p1,x,y,z,0x80,0x80,0x100)
   finds a blocking entity — if it's p2, return p2 (hit), any other -> 0.
   Runs `steps` steps, else returns 0.

   PARKED (~85%, first diff 0x8): the PARAM-PROMOTION wall (see 0x35d08), both
   directions at once. Target promotes p1 -> EBP for the whole fn but keeps
   dist memory-homed (scratch ECX CSE for the cmp+div, slot reload for the loop
   bound); ours refuses to promote p1 (explicit copy lands in EDI + spills,
   register hint worse) yet promotes dist -> ESI. Mid-section and loop body
   align modulo the cascading register encodings. int-dist + (short) casts give
   the target's 32-bit div-by-0x80 idiom; the q==p2 branch layout and the
   sub+cwde arg forms are correct. */
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int sum_of_squares_call(int a, int b);
extern unsigned char vec_to_angle(int dx, int dy);
extern int passability_4corner(int x, int y, int z);
extern unsigned char *FUN_00011d68(unsigned char *p, int x, int y, int z,
                                   int rx, int ry, int rz);

unsigned char *los_trace_far(unsigned char *p1, unsigned char *p2, int dist)
{
    int i;
    unsigned char dir2;
    unsigned char dir1;
    short x, y, z;
    int d1, d2;
    unsigned char *q;

    d1 = sum_of_squares_call((short)(*(short *)(p2 + 4) - *(short *)(p1 + 4)),
                      (short)(*(short *)(p2 + 6) - *(short *)(p1 + 6)));
    if ((short)d1 >= (short)dist)
        return 0;
    dist = (short)dist / 0x80;
    if ((short)dist < 1)
        return 0;
    y = *(short *)(p1 + 6);
    x = *(short *)(p1 + 4);
    z = *(short *)(p1 + 8);
    dir1 = vec_to_angle((short)(*(short *)(p2 + 4) - x),
                        (short)(*(short *)(p2 + 6) - y));
    d2 = sum_of_squares_call((short)(*(short *)(p2 + 4) - x),
                      (short)(*(short *)(p2 + 6) - y));
    dir2 = vec_to_angle((short)(*(short *)(p2 + 8) - z), (short)d2);
    for (i = 0; (short)i < (short)dist; i++) {
        if ((short)passability_4corner(x, y, (short)(z + 0x80)) != 0)
            return 0;
        q = FUN_00011d68(p1, x, y, z, 0x80, 0x80, 0x100);
        if (q != 0) {
            if (q == p2)
                return q;
            return 0;
        }
        x += g_dir_dx[dir1] * 0x80 >> 8;
        y += g_dir_dy[dir1] * 0x80 >> 8;
        z += g_dir_dx[dir2] * 0x80 >> 8;
    }
    return 0;
}
