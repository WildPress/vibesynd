/* framed call-then-cond-call @ 0x3b8cd: p=g1(0); if(p) p=g2(a,b,c,p); return p; */
extern void *FUN_0003da4c(int z);
extern void *FUN_0003b7e8(int a, int b, int c, void *p);
void *FUN_0003b8cd(int a, int b, int c)
{
    void *p = FUN_0003da4c(0);
    if (p)
        p = FUN_0003b7e8(a, b, c, p);
    return p;
}
