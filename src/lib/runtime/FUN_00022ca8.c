/* @ 0x22ca8 (32B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00022ca8_0(void);
#pragma aux __db_FUN_00022ca8_0 = "db 83" "db 49" "db 219" "db 49" "db 192" "db 102" "db 137" "db 216" "db 80" "db 232" "db 162" "db 4" "db 0" "db 0" "db 67" "db 131" "db 196" "db 4" "db 102" "db 131" "db 251" "db 8" "db 114" "db 235" "db 91" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00022ca8 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00022ca8(void) {
    __db_FUN_00022ca8_0();
}
