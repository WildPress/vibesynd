/* @ 0x3c74f -- heap boundary check: if last block's next-ptr hits the break, return it. */
extern int g_bfc4;
extern int *g_bfec;
extern int g_c2ac;
int FUN_0003c74f(void)
{
    if (g_bfc4 == 0) return 0;
    if ((int)g_bfec + *g_bfec + 8 == g_c2ac) return *g_bfec;
    return 0;
}
