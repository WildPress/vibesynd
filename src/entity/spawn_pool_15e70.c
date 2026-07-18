/* frameless @ 0x1c178: spawn into the g_15e70 pool (128 records of 0x1e bytes). For each
   free slot (in-use flag +0x18 == 0), stamp the coords (+4/+6/+8 = X/Y/Z from param_1..3),
   mark it in-use (type 3), clear flags/links, insert it into the spatial grid via
   grid_insert_object(p, x, y, z), then set type/frame +0x19 = 0x29 and link +0x1c = param_4.
   (Written straight from docs/object-model.md -- coords signed short, flag/type bytes.) */
extern unsigned char g_15e70[];
extern unsigned char g_16d70[];
extern void grid_insert_object(unsigned char *p, int x, int y, int z);
void spawn_pool_15e70(unsigned short param_1, unsigned short param_2, unsigned short param_3, unsigned short param_4)
{
    unsigned char *p = g_15e70;
    if (p < g_16d70)
    do {
        if (p[0x18] == 0) {
            *(unsigned short *)(p + 6) = param_2;
            *(unsigned short *)(p + 8) = param_3;
            p[0x18] = 3;
            *(unsigned short *)(p + 4) = param_1;
            *(unsigned short *)(p + 0xa) = 0;
            *(unsigned short *)(p + 0x10) = 0;
            *(unsigned short *)(p + 0x12) = 0xffff;
            grid_insert_object(p, *(short *)(p + 4), *(short *)(p + 6), *(short *)(p + 8));
            p[0x19] = 0x29;
            *(unsigned short *)(p + 0x1c) = param_4;
        }
        p += 0x1e;
    } while (p < g_16d70);
}
