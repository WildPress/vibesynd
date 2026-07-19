/* PARKED near-miss (NOT matched) -- draw-list walk over the g_rec5_table pool.
   Register-allocation/CSE wall (~133-150/183-165): the target double-loads g_rec5_table
   into a 2nd callee-saved reg (spilling EBP) and scales the ushort* index with an
   in-place ADD EAX,EAX; every recipe CSE-merges g_rec5_table to one load and folds the
   scale into LEA [EAX*2], so it never spills. -4s is the right convention; -4r
   diverges at byte 0. Logic is verified correct. Sibling pair: 0x1b858 / 0x1b798. */
/* @ 0x1b798 (183B): sibling of 0x1b858. Read the 8-byte record at
   g_rec8_table + param_1*8; if its byte field +4 is set, record_max(field,0x7f).
   Then walk the g_rec5_table pool list (5-ushort records [val,x,y,arg,next]); for
   every node with val+g_5314 > g_5314, emit draw_sprite_surf((short)(x+param_2),
   (short)(y+param_3), val+g_5314, arg). Follow next until p <= g_rec5_table.
   Stack-call (-4s). */
extern unsigned char  *g_rec8_table;
extern unsigned short *g_rec5_table;
extern unsigned int    g_5314;
extern void record_max(unsigned char, unsigned char);
extern void draw_sprite_surf(int, int, unsigned int, unsigned int);

void FUN_0001b798(unsigned short param_1, short param_2, short param_3)
{
    unsigned char  *rec;
    unsigned short *p;

    rec = g_rec8_table + (unsigned)param_1 * 8;
    if (rec[4] != 0)
        record_max(rec[4], 0x7f);
    p = g_rec5_table + (unsigned)*(unsigned short *)rec * 5;
    if (g_rec5_table < p)
    do {
        if (*p + g_5314 > g_5314)
            draw_sprite_surf((short)(p[1] + param_2), (short)(p[2] + param_3),
                         *p + g_5314, p[3]);
        p = g_rec5_table + (unsigned)p[4] * 5;
    } while (g_rec5_table < p);
}
