/* call + tail-call @ 0x00025338 (10B): call 0x254a8 ; jmp 0x28b88 */
extern void FUN_000254a8(void);
extern void FUN_00028b88(void);
void FUN_00025338(void)
{
    FUN_000254a8();
    FUN_00028b88();
}
