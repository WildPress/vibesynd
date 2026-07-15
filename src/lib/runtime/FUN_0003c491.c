/* @ 0x3c491 -- framed CLIB helper: if(flag) p = FUN_0003c4b9(p); return p. */
extern int FUN_0003c4b9(int p);
int FUN_0003c491(int p, int flag)
{
    if (flag != 0)
        p = FUN_0003c4b9(p);
    return p;
}
