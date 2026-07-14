/* @ 0x402e0 (7B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000402e0_0(void);
#pragma aux __db_FUN_000402e0_0 = "db 255" "db 36" "db 149" "db 159" "db 43" "db 3" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_000402e0 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_000402e0(void) { __db_FUN_000402e0_0(); }
