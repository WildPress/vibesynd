/* framed cond-call w/ field-mask @ 0x3b972: p=g1(c); if(p){ p[3]&=0x4000; g2(a,b,0,p);} */
extern int *getstream(int c);
extern void fopen_core(int a, int b, int z, int *p);
void fopen(int a, int b, int c)
{
    int *p = getstream(c);
    if (p) {
        p[3] &= 0x4000;
        fopen_core(a, b, 0, p);
    }
}
