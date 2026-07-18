/* @ 0x4d442 (15B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004d442_0(void);
#pragma aux __db_FUN_0004d442_0 = "db 180" "db 1" "db 205" "db 22" "db 116" "db 6" "db 180" "db 0" "db 205" "db 22" "db 235" "db 2" "db 176" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux poll_key modify [eax ebx ecx edx esi edi ebp];
void poll_key(void) { __db_FUN_0004d442_0(); }
