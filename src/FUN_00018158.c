/* FUN_00018158 @ 0x18158 - alloc/init sequence; sets g_3eec error code. */
extern unsigned int g_3eec;
extern int FUN_000180f8(int size);
extern int FUN_0001aa74(int h);
extern int FUN_0003a579(int size, int mode);
extern void FUN_0004aa59(int h);
extern int FUN_0003a7c4(int a, int b, int c);
extern void FUN_0004b859(int h);
extern void FUN_0003c89d(int h);

#pragma aux FUN_00018158 modify [eax ecx edx ebx];

int FUN_00018158(int size, int p2)
{
    int h1, h2, h3, r;

    g_3eec = 0;
    h1 = FUN_000180f8(size);
    if (h1 == -1) {
        g_3eec = 3;
        return 0;
    }
    h2 = p2;
    if (h2 == 0)
        h2 = FUN_0001aa74(h1);
    if (h2 == 0) {
        g_3eec = 2;
        return 0;
    }
    h3 = FUN_0003a579(size, 0x200);
    if (h3 == -1) {
        FUN_0004aa59(h2);
        g_3eec = 3;
        return 0;
    }
    r = FUN_0003a7c4(h3, h2, h1);
    if (r != h1) {
        FUN_0004b859(h2);
        g_3eec = 5;
        return 0;
    }
    FUN_0003c89d(h3);
    return h2;
}
