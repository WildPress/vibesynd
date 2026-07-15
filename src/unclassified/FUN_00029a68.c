/* frameless table-lookup call @ 0x29a68: g(0x10564, tbl_4480[g_language]) */
extern unsigned char g_language;
extern int tbl_4480[];
extern void FUN_000299c8(int a, int b);
void FUN_00029a68(void)
{
    FUN_000299c8(0x10564, tbl_4480[g_language]);
}
