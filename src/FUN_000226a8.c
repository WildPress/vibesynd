/* frameless init @ 0x226a8: r=g1(p->w4,p->w6,p->w8); if(r){ init fields; g2(r,p);} return r */
struct P226 { char pad4[4]; short w4; short w6; short w8; };
struct S226 {
    char pad0[0xe];
    short we;              /* 0x0e */
    char pad10[2];
    short w12;             /* 0x12 */
    short w14;             /* 0x14 */
    char pad16[3];
    unsigned char b19;    /* 0x19 */
    unsigned char b1a;    /* 0x1a */
};
extern struct S226 *FUN_00022ba8(int a, int b, int c);
extern void FUN_00037658(struct S226 *r, struct P226 *p);
struct S226 *FUN_000226a8(struct P226 *p, unsigned char param2, int param3)
{
    struct S226 *r = FUN_00022ba8(p->w4, p->w6, p->w8);
    if (r) {
        r->w12 = (short)0xffff;
        r->b1a = 0;
        r->we = 0;
        r->b19 = param2;
        r->w14 = (short)param3;
        FUN_00037658(r, p);
    }
    return r;
}
