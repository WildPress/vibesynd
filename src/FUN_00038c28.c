/* guarded alloc/read helper @ 0x38c28 */
extern signed char g_11e34;
extern signed char g_11e35;
extern int g_11e36;
extern unsigned short g_11e3a;

extern void FUN_0003b407(void *p);
extern void FUN_0003b420(void *dst, int n, int f, void *p);
extern void FUN_0003b594(void *p, int a, int b);
extern void *FUN_0003aa74(unsigned n);

void *FUN_00038c28(unsigned char *p, int a1, int a2)
{
    void *r;

    if (p == 0)
        return 0;

    FUN_0003b407(p);
    do {
        FUN_0003b420(&g_11e34, 6, 1, p);
        if (g_11e35 == -1)
            return 0;
    } while (g_11e35 != (unsigned short)a1 || g_11e34 != (unsigned short)a2);

    FUN_0003b594(p, g_11e36, 0);
    FUN_0003b420(&g_11e3a, 2, 1, p);
    r = FUN_0003aa74((unsigned short)g_11e3a);
    *(unsigned short *)r = g_11e3a;
    FUN_0003b420((unsigned char *)r + 2, g_11e3a - 2, 1, p);
    if (p[0xc] & 0x20)
        return 0;
    return r;
}
