/* @ 0x4d352 (65B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004d352_0(void);
#pragma aux __db_FUN_0004d352_0 = "db 85" "db 139" "db 236" "db 102" "db 83" "db 102" "db 81" "db 102" "db 82" "db 51" "db 192" "db 102" "db 139" "db 77" "db 8" "db 102" "db 11" "db 201" "db 116" "db 37" "db 102" "db 15" "db 189" "db 193" "db 102" "db 139" "db 28" "db 69" "db 129" "db 252" "db 3" "db 0" "db 102" "db 139" "db 193" "db 102" "db 51" "db 210" "db 102" "db 247" "db 243" "db 102" "db 59" "db 195" "db 125" "db 8" "db 102" "db 3" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004d352_1(void);
#pragma aux __db_FUN_0004d352_1 = "db 216" "db 102" "db 209" "db 235" "db 235" "db 234" "db 102" "db 139" "db 195" "db 102" "db 90" "db 102" "db 89" "db 102" "db 91" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux isqrt16 modify [eax ebx ecx edx esi edi ebp];
void isqrt16(void) { __db_FUN_0004d352_0(); __db_FUN_0004d352_1(); }
