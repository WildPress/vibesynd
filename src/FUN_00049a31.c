/* @ 0x49a31 (70B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00049a31_0(void);
#pragma aux __db_FUN_00049a31_0 = "db 85" "db 139" "db 236" "db 80" "db 81" "db 86" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 2" "db 116" "db 30" "db 139" "db 117" "db 8" "db 139" "db 69" "db 12" "db 102" "db 185" "db 64" "db 6" "db 137" "db 6" "db 137" "db 70" "db 4" "db 137" "db 70" "db 8" "db 137" "db 70" "db 12" "db 131" "db 198" "db 80" "db 102" "db 73" "db 117" "db 238" "db 235" "db 20" "db 246" "db 5" "db 5" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00049a31_1(void);
#pragma aux __db_FUN_00049a31_1 = "db 1" "db 0" "db 0" "db 4" "db 116" "db 2" "db 235" "db 9" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 1" "db 116" "db 0" "db 94" "db 89" "db 88" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00049a31 modify [eax ebx ecx edx esi edi ebp];
void FUN_00049a31(void) { __db_FUN_00049a31_0(); __db_FUN_00049a31_1(); }
