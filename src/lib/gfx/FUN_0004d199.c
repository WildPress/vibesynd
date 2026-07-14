/* @ 0x4d199 (66B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004d199_0(void);
#pragma aux __db_FUN_0004d199_0 = "db 85" "db 139" "db 236" "db 81" "db 87" "db 139" "db 77" "db 16" "db 247" "db 193" "db 3" "db 0" "db 0" "db 0" "db 116" "db 35" "db 247" "db 193" "db 2" "db 0" "db 0" "db 0" "db 116" "db 12" "db 139" "db 125" "db 8" "db 139" "db 69" "db 12" "db 243" "db 170" "db 95" "db 89" "db 201" "db 195" "db 209" "db 233" "db 139" "db 125" "db 8" "db 139" "db 69" "db 12" "db 243" "db 102" "db 171" "db 95" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004d199_1(void);
#pragma aux __db_FUN_0004d199_1 = "db 89" "db 201" "db 195" "db 193" "db 233" "db 2" "db 139" "db 125" "db 8" "db 139" "db 69" "db 12" "db 243" "db 171" "db 95" "db 89" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0004d199 modify [eax ebx ecx edx esi edi ebp];
void FUN_0004d199(void) { __db_FUN_0004d199_0(); __db_FUN_0004d199_1(); }
