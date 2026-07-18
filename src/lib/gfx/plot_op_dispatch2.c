/* @ 0x404b8 (7B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000404b8_0(void);
#pragma aux __db_FUN_000404b8_0 = "db 255" "db 36" "db 149" "db 119" "db 45" "db 3" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux plot_op_dispatch2 modify [eax ebx ecx edx esi edi ebp] aborts;
void plot_op_dispatch2(void) { __db_FUN_000404b8_0(); }
