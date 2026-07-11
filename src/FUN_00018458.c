/* frameless global-update + call @ 0x18458: g_3ef0 += a; FUN_0003aa74(a + 0x100) */
extern int g_3ef0;
extern void FUN_0003aa74(int x);
void FUN_00018458(int a)
{
    g_3ef0 += a;
    FUN_0003aa74(a + 0x100);
}
