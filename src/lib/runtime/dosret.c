/* @ 0x3c479 -- framed CLIB helper: if(b) return FUN_0003c491(a,b); else b. */
extern int FUN_0003c491(int a, int b);
int dosret(int a, int b)
{
    if (b != 0)
        return FUN_0003c491(a, b);
    return b;
}
