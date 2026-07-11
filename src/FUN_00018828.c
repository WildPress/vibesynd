/* frameless call-seq @ 0x18828: conditional 3-call block, then g4(a,b,0x40) */
extern int FUN_0003addb(int a, int b);
extern int FUN_0003adf0(int a, int b);
extern void FUN_0003a89d(int x);
extern void FUN_0003a598(int a, int b, int c);
void FUN_00018828(int a, int b)
{
    if (b == 0x222) {
        int r = FUN_0003addb(a, 0x1c0);
        FUN_0003adf0(r, 0x200);
        FUN_0003a89d(r);
    }
    FUN_0003a598(a, b, 0x40);
}
