/* @ 0x46188 (144B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00046188_0(void);
#pragma aux __db_FUN_00046188_0 = "db 184" "db 68" "db 225" "db 0" "db 0" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" "db 20" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" "db 40" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" "db 60" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" "db 80" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" "db 100" "db 0" "db 0" "db 0" "db 0" "db 199" "db 64" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00046188_1(void);
#pragma aux __db_FUN_00046188_1 = "db 120" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 140" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 160" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 180" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 200" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 220" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00046188_2(void);
#pragma aux __db_FUN_00046188_2 = "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 240" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 4" "db 1" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 24" "db 1" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 199" "db 128" "db 44" "db 1" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00046188 modify [eax ebx ecx edx esi edi ebp];
void FUN_00046188(void) { __db_FUN_00046188_0(); __db_FUN_00046188_1(); __db_FUN_00046188_2(); }
