/* frameless slot scan+init @ 0x22b38: find free slot in g_15e70[256] (stride 0x1e),
   fill it, call g(s, w4,w6,w8), return s; else 0. Uses the extern array (keeps the
   bounds guard the original has); the array-bound relocation is masked via OMF fixups. */
struct slot22 {
    char pad4[4];
    short w4; short w6; short w8; short wa;   /* 4,6,8,a */
    char pad_c[4];
    short w10; short w12;                     /* 10,12 */
    char pad14[4];
    unsigned char b18;                        /* 18 */
    char pad19[5];                            /* -> stride 0x1e */
};
extern struct slot22 g_15e70[256];
extern void FUN_00026e18(struct slot22 *s, int a, int b, int c);
struct slot22 *FUN_00022b38(int p1, int p2, int p3)
{
    struct slot22 *s;
    for (s = g_15e70; s < &g_15e70[256]; s++) {
        if (s->b18 == 0) {
            s->w4 = (short)p1;
            s->w6 = (short)p2;
            s->w8 = (short)p3;
            s->b18 = 3;
            s->wa = 0;
            s->w10 = 0;
            s->w12 = (short)0xffff;
            FUN_00026e18(s, s->w4, s->w6, s->w8);
            return s;
        }
    }
    return 0;
}
