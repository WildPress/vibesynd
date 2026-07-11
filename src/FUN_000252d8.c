/* frameless linear call-seq @ 0x252d8: guard on two flags, then PIT timer setup + d_setvec */
extern unsigned char g_10b4a, g_10b49;
extern void FUN_0003b22d(int port, int val);
extern void FUN_0003b273(int a, int b, int c);
extern int g_df08;
extern unsigned short g_df0c;
void FUN_000252d8(void)
{
    if (g_10b4a != 0)
        return;
    if (g_10b49 != 0)
        return;
    FUN_0003b22d(0x43, 0x36);
    FUN_0003b22d(0x40, 0);
    FUN_0003b22d(0x40, 0);
    FUN_0003b273(8, g_df08, g_df0c);
}
