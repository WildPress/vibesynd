/* PARKED near-miss (NOT matched) @ 0x1b858 (165B) -- draw-list walk over the g_rec5_table pool.
   Sibling of 0x1b798. Logic verified instruction-exact (the whole draw_sprite_buf call region
   is byte-identical masked). Plain 3-arg STACK-call fn (-4s); NOT __thiscall (the decompiler
   was wrong -- params come from [ESP+..], and -4r diverges at byte 0).
   WALL (register-allocation/CSE, best 121/165, 5000 cpermute variants): the target reloads
   g_5314 each iteration into callee-saved EBP (4 pushes EBX/ESI/EDI/EBP) and reloads g_rec5_table
   uncached; our -oneatx build (relaxed alias) uses EDX (3 pushes) and CSE-merges the global
   loads, so the arg stack offsets shift by +4. Not reachable from C under the required recipe.
   Each record is 5 ushorts [val,x,y,arg,next]; emit draw_sprite_buf for nodes with val+g_5314>g_5314. */
extern unsigned char  *g_rec8_table;
extern unsigned short *g_rec5_table;
extern unsigned int    g_5314;
extern void draw_sprite_buf(int, int, unsigned int, unsigned int);

void FUN_0001b858(unsigned short param_1, short param_2, short param_3)
{
    unsigned short *p;

    p = g_rec5_table + (unsigned)*(unsigned short *)(g_rec8_table + (unsigned)param_1 * 8) * 5;
    if (g_rec5_table < p)
    do {
        if (*p + g_5314 > g_5314)
            draw_sprite_buf((short)(p[1] + param_2), (short)(p[2] + param_3),
                         *p + g_5314, p[3]);
        p = g_rec5_table + (unsigned)p[4] * 5;
    } while (g_rec5_table < p);
}
