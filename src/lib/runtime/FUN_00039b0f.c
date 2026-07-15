/* @ 0x39b0f (70B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039b0f_0(void);
#pragma aux __db_FUN_00039b0f_0 = "db 184" "db 126" "db 0" "db 0" "db 0" "db 233" "db 147" "db 247" "db 255" "db 255" "db 184" "db 127" "db 0" "db 0" "db 0" "db 233" "db 137" "db 247" "db 255" "db 255" "db 184" "db 128" "db 0" "db 0" "db 0" "db 233" "db 127" "db 247" "db 255" "db 255" "db 184" "db 129" "db 0" "db 0" "db 0" "db 233" "db 117" "db 247" "db 255" "db 255" "db 184" "db 130" "db 0" "db 0" "db 0" "db 233" "db 107" "db 247" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039b0f_1(void);
#pragma aux __db_FUN_00039b0f_1 = "db 255" "db 255" "db 184" "db 131" "db 0" "db 0" "db 0" "db 233" "db 97" "db 247" "db 255" "db 255" "db 184" "db 132" "db 0" "db 0" "db 0" "db 233" "db 87" "db 247" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00039b0f modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00039b0f(void) {
    __db_FUN_00039b0f_0();
    __db_FUN_00039b0f_1();
}
