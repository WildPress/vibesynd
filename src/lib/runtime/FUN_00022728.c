/* @ 0x22728 (64B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00022728_0(void);
#pragma aux __db_FUN_00022728_0 = "db 104" "db 8" "db 63" "db 0" "db 0" "db 232" "db 182" "db 92" "db 255" "db 255" "db 131" "db 196" "db 4" "db 104" "db 68" "db 65" "db 0" "db 0" "db 232" "db 169" "db 92" "db 255" "db 255" "db 138" "db 37" "db 74" "db 11" "db 1" "db 0" "db 131" "db 196" "db 4" "db 132" "db 228" "db 116" "db 18" "db 232" "db 55" "db 58" "db 1" "db 0" "db 104" "db 192" "db 64" "db 0" "db 0" "db 232" "db 141" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00022728_1(void);
#pragma aux __db_FUN_00022728_1 = "db 92" "db 255" "db 255" "db 131" "db 196" "db 4" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 82" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00022728 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00022728(void) {
    __db_FUN_00022728_0();
    __db_FUN_00022728_1();
}
