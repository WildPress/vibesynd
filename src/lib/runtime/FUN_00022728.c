/* @ 0x22728 -- init call sequence with a conditional third step. */
extern void FUN_000183e8(int a);
extern void FUN_00036188(void);
extern char g_10b4a;

void FUN_00022728(void)
{
    FUN_000183e8(0x3f08);
    FUN_000183e8(0x4144);
    if (g_10b4a != 0) {
        FUN_00036188();
        FUN_000183e8(0x40c0);
    }
}
