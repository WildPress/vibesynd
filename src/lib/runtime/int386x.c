/* @ 0x3d4bc (55B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d4bc_0(void);
#pragma aux __db_FUN_0003d4bc_0 = "db 85" "db 6" "db 83" "db 30" "db 82" "db 232" "db 45" "db 0" "db 0" "db 0" "db 30" "db 87" "db 139" "db 236" "db 139" "db 125" "db 8" "db 142" "db 93" "db 12" "db 137" "db 7" "db 137" "db 95" "db 4" "db 137" "db 79" "db 8" "db 137" "db 87" "db 12" "db 137" "db 119" "db 16" "db 143" "db 71" "db 20" "db 27" "db 192" "db 137" "db 71" "db 24" "db 88" "db 91" "db 91" "db 91" "db 102" "db 137" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d4bc_1(void);
#pragma aux __db_FUN_0003d4bc_1 = "db 67" "db 6" "db 140" "db 3" "db 7" "db 93" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux int386x modify [eax ebx ecx edx esi edi ebp];
void int386x(void) { __db_FUN_0003d4bc_0(); __db_FUN_0003d4bc_1(); }
