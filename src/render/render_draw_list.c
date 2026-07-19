/* frameless @ 0x1b858  render_draw_list: walk a linked display list and draw each visible node.
   Start index = g_rec8_table[p1].word0; follow the 10-byte nodes in g_rec5_table
   via the +8 next-index until the index reaches 0 (node ptr <= base). For each node
   whose biased screen y (word0 + g_5314) exceeds g_5314, call the sprite blit 0x4a63a
   with the node's x/y offsets biased by the caller's origin (dx_base, dy_base).

   DUP RESOLVED: an identical stale copy at src/unclassified/render_draw_list.c was deleted
   (both spellings were logically byte-equivalent; find(1) compiled the render copy first,
   so removing the dup left the distance unchanged). This is the surviving copy.

   PARKED at dist 66. Logic + unsigned guards + call-arg order all match; the gap is two
   stacked TOOLCHAIN codegen-ties with no faithful C lever:
     1. register count -- the original spends a 4th callee-saved reg (push ebp) to hold the
        per-iteration g_5314 and puts dx_base/dy_base in edi/esi; we use 3 callee-saved
        (esi/edi roles swapped, g_5314 in a volatile eax). Only 3 values cross the call, so
        no source spelling forces the 4th push.
     2. CSE -- the original reloads g_rec5_table on every reference (cmp reg,reg); our 9.5b
        under -oneatx merges the two reads of the same global. Both read the identical global,
        so no faithful spelling changes it. (`volatile` cuts to 62 but overshoots to cmp
        reg,[mem] and mislabels a plain global; caching g_5314 before the loop cuts to 56 but
        re-samples the global at the wrong point -- both rejected as byte-tricks.) */
extern unsigned char *g_rec8_table;
extern unsigned char *g_rec5_table;
extern int g_5314;
extern void draw_sprite_buf(int x, int y, int sy, int w);

void render_draw_list(unsigned short p1, int dx_base, int dy_base)
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
