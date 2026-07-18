/* framed cond-call w/ field-mask @ 0x3b972: p=g1(c); if(p){ p[3]&=0x4000; g2(a,b,0,p);} */
extern int *FUN_0003b90d(int c);
extern void FUN_0003b7e8(int a, int b, int z, int *p);
void fopen(int a, int b, int c)
{
    int *p = FUN_0003b90d(c);
    if (p) {
        p[3] &= 0x4000;
        FUN_0003b7e8(a, b, 0, p);
    }
}
