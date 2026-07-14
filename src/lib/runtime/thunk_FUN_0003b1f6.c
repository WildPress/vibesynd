/* @ 0x3b1d1 (2B) -- db-transcription (hand-asm/library). */

extern void __db_thunk_FUN_0003b1f6_0(void);
#pragma aux __db_thunk_FUN_0003b1f6_0 = "db 235" "db 35" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux thunk_FUN_0003b1f6 modify [eax ebx ecx edx esi edi ebp] aborts;
void thunk_FUN_0003b1f6(void) {
    __db_thunk_FUN_0003b1f6_0();
}
