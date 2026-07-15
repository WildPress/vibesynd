/* @ 0x26998 (32B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00026998_0(void);
#pragma aux __db_FUN_00026998_0 = "db 232" "db 123" "db 0" "db 0" "db 0" "db 232" "db 182" "db 174" "db 0" "db 0" "db 232" "db 17" "db 23" "db 1" "db 0" "db 232" "db 28" "db 89" "db 255" "db 255" "db 232" "db 215" "db 38" "db 0" "db 0" "db 233" "db 34" "db 6" "db 1" "db 0" "db 139" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00026998 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00026998(void) {
    __db_FUN_00026998_0();
}
