/* frameless @ 0x2e5f8: line-of-sight trace from entity p1 toward entity p2
   (sibling of 0x2e808, with extra head checks and a richer hit branch).
   Head: if p1 has flag 2 at +0x1c and p1/p2 are in the same pool-A group of 8
   ((ptr-0x8110)/0x5c/8 equal), fail. If p2's type byte (+0x19) == 0x17, fail.
   d1 = dist(p2-p1); if d1 >= range, fail. steps = range/0x80 (>= 1).
   March x/y/z from p1's coords in half-tile increments (g_dir_dx/g_dir_dy * 0x80
   >> 8); each step must be passable (FUN_f898 == 0); FUN_11d68 finds a
   blocking entity q: q must be class 1 (+0x18), q == p2 -> return q (hit),
   q == p1 -> ignore (keep marching), any other q -> return q.

   PARKED near-miss, length-EXACT 519/519, first diff 0xc3. Semantics + all
   structure byte-correct; the ONLY defect is a spill-slot pair swap: the target
   lays the six esp locals out z@[esp+0], i@[esp+4], x@[esp+8], y@[esp+0xc],
   dir2@[esp+0x10], dir1@[esp+0x14]; ours packs the coords into 0/8/4 and lands
   the loop counter i at [esp+0xc], i.e. y and i are transposed. That single
   transposition cascades into ~40 modrm-disp bytes across the marching loop.
   TRIED (all provably inert or worse): decl orders z/i/x/y, x/y/z/i, z/x/y,
   early `i = 0`, int-typed counter (changes frame to 0x14, breaks 16-bit cmp).
   This is the 0x338d8 spill-slot-order wall — allocator-internal, decl-order
   proven byte-inert here; not source-reachable, cpermute can't touch slots. */
extern unsigned char g_pool_a[];
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int sum_of_squares_call(int a, int b);
extern unsigned char vec_to_angle(int dx, int dy);
extern int passability_4corner(int x, int y, int z);
extern unsigned char *FUN_00011d68(unsigned char *p, int x, int y, int z,
                                   int rx, int ry, int rz);

unsigned char *los_trace(unsigned char *p1, unsigned char *p2, int dist)
{
    int d1, d2;
    unsigned char *q;
    short x;
    short y;
    short z;
    unsigned short i;
    unsigned char dir1;
    unsigned char dir2;

    if (*(p1 + 0x1c) & 2) {
        if ((p1 - g_pool_a) / 0x5c / 8 == (p2 - g_pool_a) / 0x5c / 8)
            return 0;
    }
    if (*(p2 + 0x19) == 0x17)
        return 0;
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
    i = 0;
    x = *(short *)(p1 + 4);
    y = *(short *)(p1 + 6);
    z = *(short *)(p1 + 8);
    dir1 = vec_to_angle((short)(*(short *)(p2 + 4) - x),
                        (short)(*(short *)(p2 + 6) - y));
    d2 = sum_of_squares_call((short)(*(short *)(p2 + 4) - x),
                      (short)(*(short *)(p2 + 6) - y));
    dir2 = vec_to_angle((short)(*(short *)(p2 + 8) - z), (short)d2);
    for (; (short)i < (short)dist; i++) {
        if ((short)passability_4corner(x, y, (short)(z + 0x80)) != 0)
            return 0;
        q = FUN_00011d68(p1, x, y, z, 0x80, 0x80, 0x100);
        if (q != 0) {
            if (*(q + 0x18) != 1)
                return 0;
            if (p2 == q)
                return q;
            if (p1 != q)
                return q;
        }
        x += g_dir_dx[dir1] * 0x80 >> 8;
        y += g_dir_dy[dir1] * 0x80 >> 8;
        z += g_dir_dx[dir2] * 0x80 >> 8;
    }
    return 0;
}
