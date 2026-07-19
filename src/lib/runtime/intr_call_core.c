/* @ 0x3d4f3 (36B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d4f3_0(void);
#pragma aux __db_FUN_0003d4f3_0 = "db 141" "db 52" "db 118" "db 141" "db 134" "db 76" "db 254" "db 2" "db 0" "db 80" "db 142" "db 3" "db 102" "db 139" "db 107" "db 6" "db 139" "db 7" "db 139" "db 95" "db 4" "db 139" "db 79" "db 8" "db 139" "db 87" "db 12" "db 139" "db 119" "db 16" "db 139" "db 127" "db 20" "db 142" "db 221" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux intr_call_core modify [eax ebx ecx edx esi edi ebp];
void intr_call_core(void) { __db_FUN_0003d4f3_0(); }
