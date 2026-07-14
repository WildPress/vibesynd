/* frameless @ 0x28cc8: build two local buffers, zero a word in one, call
   FUN_0003adb2(0x33, b, a); then if global g_df3c is set, call
   FUN_0003ab59(g_df3c). */
extern int FUN_0003adb2(int a, void *b, void *c);
extern void FUN_0003ab59(int x);
extern int g_df3c;
void FUN_00028cc8(void)
{
    char b[0x1c];
    char a[0x1c];
    *(short *)b = 0;
    FUN_0003adb2(0x33, b, a);
    if (g_df3c != 0)
        FUN_0003ab59(g_df3c);
}
