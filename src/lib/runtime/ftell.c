/* @ 0x3da03 (52B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003da03_0(void);
#pragma aux __db_FUN_0003da03_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 93" "db 12" "db 255" "db 115" "db 16" "db 232" "db 106" "db 207" "db 255" "db 255" "db 131" "db 196" "db 4" "db 137" "db 194" "db 131" "db 248" "db 255" "db 116" "db 24" "db 131" "db 123" "db 4" "db 0" "db 116" "db 16" "db 246" "db 67" "db 13" "db 16" "db 116" "db 7" "db 139" "db 83" "db 4" "db 1" "db 194" "db 235" "db 3" "db 43" "db 83" "db 4" "db 137" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003da03_1(void);
#pragma aux __db_FUN_0003da03_1 = "db 208" "db 93" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux ftell modify [eax ebx ecx edx esi edi ebp];
void ftell(void) { __db_FUN_0003da03_0(); __db_FUN_0003da03_1(); }
