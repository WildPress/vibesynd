/* PARKED near-miss (NOT matched) -- draw-list walk over the g_5340 pool.
   Register-allocation/CSE wall (~133-150/183-165): the target double-loads g_5340
   into a 2nd callee-saved reg (spilling EBP) and scales the ushort* index with an
   in-place ADD EAX,EAX; every recipe CSE-merges g_5340 to one load and folds the
   scale into LEA [EAX*2], so it never spills. -4s is the right convention; -4r
   diverges at byte 0. Logic is verified correct. Sibling pair: 0x1b858 / 0x1b798. */
/* @ 0x1b798 (183B): sibling of 0x1b858. Read the 8-byte record at
   g_5338 + param_1*8; if its byte field +4 is set, FUN_00035f28(field,0x7f).
   Then walk the g_5340 pool list (5-ushort records [val,x,y,arg,next]); for
   every node with val+g_5314 > g_5314, emit FUN_0004a66b((short)(x+param_2),
   (short)(y+param_3), val+g_5314, arg). Follow next until p <= g_5340.
   Stack-call (-4s). */
extern unsigned char  *g_5338;
extern unsigned short *g_5340;
extern unsigned int    g_5314;
extern void FUN_00035f28(unsigned char, unsigned char);
extern void FUN_0004a66b(int, int, unsigned int, unsigned int);

void FUN_0001b798(unsigned short param_1, short param_2, short param_3)
{
    unsigned char  *rec;
    unsigned short *p;

    rec = g_5338 + (unsigned)param_1 * 8;
    if (rec[4] != 0)
        FUN_00035f28(rec[4], 0x7f);
    p = g_5340 + (unsigned)*(unsigned short *)rec * 5;
    if (g_5340 < p)
    do {
        if (*p + g_5314 > g_5314)
            FUN_0004a66b((short)(p[1] + param_2), (short)(p[2] + param_3),
                         *p + g_5314, p[3]);
        p = g_5340 + (unsigned)p[4] * 5;
    } while (g_5340 < p);
}
