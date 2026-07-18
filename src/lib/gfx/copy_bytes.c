/* @ 0x4d1db (70B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004d1db_0(void);
#pragma aux __db_FUN_0004d1db_0 = "db 85" "db 139" "db 236" "db 81" "db 87" "db 86" "db 139" "db 77" "db 16" "db 247" "db 193" "db 3" "db 0" "db 0" "db 0" "db 116" "db 37" "db 247" "db 193" "db 2" "db 0" "db 0" "db 0" "db 116" "db 13" "db 139" "db 117" "db 8" "db 139" "db 125" "db 12" "db 243" "db 164" "db 94" "db 95" "db 89" "db 201" "db 195" "db 209" "db 233" "db 139" "db 117" "db 8" "db 139" "db 125" "db 12" "db 243" "db 102" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004d1db_1(void);
#pragma aux __db_FUN_0004d1db_1 = "db 165" "db 94" "db 95" "db 89" "db 201" "db 195" "db 193" "db 233" "db 2" "db 139" "db 117" "db 8" "db 139" "db 125" "db 12" "db 243" "db 165" "db 94" "db 95" "db 89" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux copy_bytes modify [eax ebx ecx edx esi edi ebp];
void copy_bytes(void) { __db_FUN_0004d1db_0(); __db_FUN_0004d1db_1(); }
