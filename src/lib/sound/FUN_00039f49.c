/* frameless linear call @ 0x39f49: g(g_bdd0, g_back_buf, (u16)g_bddc - 6) */
extern unsigned short g_bddc;
extern int g_back_buf;
extern int g_bdd0;
extern void FUN_0003a7c4(int a, int b, int c);
void FUN_00039f49(void)
{
    FUN_0003a7c4(g_bdd0, g_back_buf, g_bddc - 6);
}
