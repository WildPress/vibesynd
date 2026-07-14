/* @ 0x00015d98 (20B): call three functions, tail-call the fourth.
 *   call 0x3eda6; call 0x355d8; call 0x3ee21; jmp 0x35538 */
extern void FUN_0003eda6(void);
extern void FUN_000355d8(void);
extern void FUN_0003ee21(void);
extern void FUN_00035538(void);
void FUN_00015d98(void)
{
    FUN_0003eda6();
    FUN_000355d8();
    FUN_0003ee21();
    FUN_00035538();
}
