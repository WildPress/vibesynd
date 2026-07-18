/* frameless @ 0x1b858: walk a linked display list and draw each visible node.
   Start index = g_rec8_table[p1].word0; follow the 10-byte nodes in g_rec5_table
   via the +8 next-index until the index reaches 0 (node ptr <= base). For each node
   with a non-zero screen y (word0 + g_5314), call the sprite blit 0x4a63a with the
   node's x/y offsets biased by the caller's origin (dx_base, dy_base).

   PARKED near-miss (~153/165). Logic + unsigned guards match; the wall is whole-function
   register allocation: the original spends a 4th callee-saved register (push ebp) to hold
   g_5314 per iteration and assigns dx_base/dy_base to edi/esi (we get the esi/edi roles
   swapped and keep g_5314 in eax, 3 callee-saved). base is live only before the call, so
   no C spelling makes 9.5b promote it to a callee-saved reg. Register-role family. */
extern unsigned char *g_rec8_table;
extern unsigned char *g_rec5_table;
extern int g_5314;
extern void draw_sprite_buf(int x, int y, int sy, int w);

void FUN_0001b858(unsigned short p1, int dx_base, int dy_base)
{
    unsigned index = *(unsigned short *)(g_rec8_table + (unsigned)p1 * 8);
    unsigned char *r = g_rec5_table + index * 10;
    while (r > g_rec5_table) {
        unsigned base = (unsigned)g_5314;
        unsigned sy = (unsigned short)*(unsigned short *)r + base;
        if (sy > base) {
            draw_sprite_buf((short)(*(unsigned short *)(r + 2) + dx_base),
                         (short)(*(unsigned short *)(r + 4) + dy_base),
                         (int)sy,
                         *(unsigned short *)(r + 6));
        }
        index = *(unsigned short *)(r + 8);
        r = g_rec5_table + index * 10;
    }
}
