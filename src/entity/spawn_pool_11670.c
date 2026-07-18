/* frameless @ 0x22ba8: spawn into the g_11670 pool (records of 0x24 bytes). Scan for a
   free slot -- in-use flag +0x18 == 0 OR >= 6 -- clear the 0x24-byte record via
   fill_bytes, stamp coords (+4/+6/+8 = param_1..3), mark in-use type 4, insert into
   the spatial grid via grid_insert_object(p, x, y, z) with coords sign-extended, return the
   slot. Sibling of spawn_pool_15e70. Bounds are EXTERN symbols so the loop cmp isn't folded. */
extern unsigned short g_11670[];
extern unsigned short g_15e70[];
extern void fill_bytes(unsigned short *p, int c, int n);
extern void grid_insert_object(unsigned short *p, int x, int y, int z);

unsigned short *spawn_pool_11670(unsigned short param_1, unsigned short param_2, unsigned short param_3)
{
    unsigned short *p = g_11670;
    while (p < g_15e70) {
        if ((signed char)p[0xc] != 0 && (signed char)p[0xc] < 6) {
            p += 0x12;
            continue;
        }
        fill_bytes(p, 0, 0x24);
        p[2] = param_1;
        p[3] = param_2;
        p[4] = param_3;
        *(unsigned char *)(p + 0xc) = 4;
        grid_insert_object(p, *(short *)(p + 2), *(short *)(p + 3), (short)param_3);
        return p;
    }
    return 0;
}
