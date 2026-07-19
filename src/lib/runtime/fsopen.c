/* framed call-then-cond-call @ 0x3b8cd: p=g1(0); if(p) p=g2(a,b,c,p); return p; */
extern void *alloc_stream(int z);
extern void *fopen_core(int a, int b, int c, void *p);
void *fsopen(int a, int b, int c)
{
    void *p = alloc_stream(0);
    if (p)
        p = fopen_core(a, b, c, p);
    return p;
}
