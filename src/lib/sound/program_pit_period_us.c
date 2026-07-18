/* @ 0x39495 (49B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039495_0(void);
#pragma aux __db_FUN_00039495_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 184" "db 0" "db 0" "db 0" "db 0" "db 129" "db 125" "db 8" "db 141" "db 214" "db 0" "db 0" "db 115" "db 17" "db 139" "db 69" "db 8" "db 187" "db 188" "db 32" "db 0" "db 0" "db 185" "db 16" "db 39" "db 0" "db 0" "db 247" "db 225" "db 247" "db 243" "db 80" "db 232" "db 168" "db 255" "db 255" "db 255" "db 131" "db 196" "db 4" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux program_pit_period_us modify [eax ebx ecx edx esi edi ebp];
void program_pit_period_us(void) {
    __db_FUN_00039495_0();
}
