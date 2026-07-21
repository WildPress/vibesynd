/* @ 0x37ff8 (74B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00037ff8_0(void);
#pragma aux __db_FUN_00037ff8_0 = "db 15" "db 191" "db 84" "db 36" "db 12" "db 82" "db 15" "db 191" "db 84" "db 36" "db 12" "db 82" "db 15" "db 191" "db 84" "db 36" "db 12" "db 82" "db 232" "db 41" "db 171" "db 254" "db 255" "db 137" "db 194" "db 131" "db 196" "db 12" "db 133" "db 192" "db 116" "db 39" "db 138" "db 68" "db 36" "db 16" "db 136" "db 66" "db 26" "db 138" "db 68" "db 36" "db 20" "db 136" "db 66" "db 27" "db 49" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00037ff8_1(void);
#pragma aux __db_FUN_00037ff8_1 = "db 138" "db 68" "db 36" "db 24" "db 102" "db 137" "db 66" "db 20" "db 138" "db 68" "db 36" "db 32" "db 136" "db 66" "db 25" "db 139" "db 68" "db 36" "db 28" "db 102" "db 137" "db 66" "db 28" "db 137" "db 208" "db 195" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00037ff8 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00037ff8(void) {
    __db_FUN_00037ff8_0();
    __db_FUN_00037ff8_1();
}
