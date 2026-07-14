/* frameless two table-lookup calls @ 0x29a28 */
extern unsigned char g_a50d;
extern int tbl_4468[];
extern int tbl_4474[];
extern void FUN_000299c8(int a, int b);
void FUN_00029a28(void)
{
    FUN_000299c8(0x10564, tbl_4468[g_a50d]);
    FUN_000299c8(0x10574, tbl_4474[g_a50d]);
}
