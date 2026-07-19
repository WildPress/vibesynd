/* frameless @ 0x2e808: line-of-sight trace from entity p1 toward entity p2.
   d1 = dist(p2-p1) via FUN_14c58; if d1 >= range, fail. steps = range/0x80
   (must be >= 1). dir1 = angle(p2-p1 in xy) via FUN_4d221, d2 = xy distance,
   dir2 = angle(dz, d2) (vertical). March x/y/z from p1's coords in half-tile
   increments (g_dir_dx/g_dir_dy direction tables * 0x80 >> 8): each step must be
   passable (FUN_f898(x,y,z+0x80) == 0); FUN_11d68(p1,x,y,z,0x80,0x80,0x100)
   finds a blocking entity — if it's p2, return p2 (hit), any other -> 0.
   Runs `steps` steps, else returns 0.

   NEAR-MATCH, EDIT-DIST 53 (was 148), first diff 0x3c. The whole setup region
   0x0..0xd4 (~212 bytes: prologue, d1 dist test, div-by-0x80, dir1/d2/dir2, loop
   entry) is now BYTE-PERFECT. Key fixes were adopting the sibling los_trace's
   exact idioms: `unsigned short i` loop counter, an `int t` temp for the
   div-by-0x80, x/y/z assigned in that order, and — critically — putting the
   counter init in the for-statement (`for (i = 0; ...)`) not a separate early
   `i = 0;`. That flips the allocator into promoting p1 -> EBP and homing the
   running coords in registers (x=ESI, y=EBX, z=EDI) with i memory-homed at
   [esp], exactly like the target. Remaining 53 is pure loop-body instruction
   scheduling (0x126..): which scratch register each half-tile step materializes
   in (target x-step lands in EDX -> `add edx,esi; mov esi,edx`; ours keeps it
   in-place `add esi,edx`, 2 bytes shorter) plus the interleave of the i inc/store
   against the shifts. Not source-reachable — the codegen-tie floor. Tried:
   update reorder y/x/z (worse, 55), `x = step + x` spelling (inert). */
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int sum_of_squares_call(int a, int b);
extern unsigned char vec_to_angle(int dx, int dy);
extern int passability_4corner(int x, int y, int z);
extern unsigned char *find_blocking_entity(unsigned char *p, int x, int y, int z,
                                   int rx, int ry, int rz);

unsigned char *los_trace_far(unsigned char *p1, unsigned char *p2, int dist)
{
    int d1, d2;
    unsigned char *q;
    short x;
    short y;
    short z;
    unsigned short i;
    unsigned char dir1;
    unsigned char dir2;

    d1 = sum_of_squares_call((short)(*(short *)(p2 + 4) - *(short *)(p1 + 4)),
                      (short)(*(short *)(p2 + 6) - *(short *)(p1 + 6)));
    if ((short)d1 >= (short)dist)
        return 0;
    {
        int t = (short)dist / 0x80;
        dist = t;
        if ((short)t < 1)
            return 0;
    }
    x = *(short *)(p1 + 4);
    y = *(short *)(p1 + 6);
    z = *(short *)(p1 + 8);
    dir1 = vec_to_angle((short)(*(short *)(p2 + 4) - x),
                        (short)(*(short *)(p2 + 6) - y));
    d2 = sum_of_squares_call((short)(*(short *)(p2 + 4) - x),
                      (short)(*(short *)(p2 + 6) - y));
    dir2 = vec_to_angle((short)(*(short *)(p2 + 8) - z), (short)d2);
    for (i = 0; (short)i < (short)dist; i++) {
        if ((short)passability_4corner(x, y, (short)(z + 0x80)) != 0)
            return 0;
        q = find_blocking_entity(p1, x, y, z, 0x80, 0x80, 0x100);
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
