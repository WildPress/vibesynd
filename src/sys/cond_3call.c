/* frameless call-seq @ 0x18828: conditional 3-call block, then g4(a,b,0x40) */
extern int open_rw_creat_trunc(int a, int b);
extern int setmode(int a, int b);
extern void close(int x);
extern void sopen(int a, int b, int c);
void cond_3call(int a, int b)
{
    if (b == 0x222) {
        int r = open_rw_creat_trunc(a, 0x1c0);
        setmode(r, 0x200);
        close(r);
    }
    sopen(a, b, 0x40);
}
