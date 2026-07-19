/* frameless global-update + call @ 0x18458: g_3ef0 += a; FUN_0003aa74(a + 0x100) */
extern int g_3ef0;
extern void FUN_0003aa74(int x);
void global_add_and_call(int a)
{
    g_3ef0 += a;
    FUN_0003aa74(a + 0x100);
}
