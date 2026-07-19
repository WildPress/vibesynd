/* @ 0x3b99e (44B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003b99e_0(void);
#pragma aux __db_FUN_0003b99e_0 = "db 85" "db 137" "db 229" "db 139" "db 85" "db 8" "db 161" "db 76" "db 30" "db 1" "db 0" "db 133" "db 192" "db 117" "db 7" "db 184" "db 255" "db 255" "db 255" "db 255" "db 235" "db 20" "db 59" "db 80" "db 4" "db 116" "db 4" "db 139" "db 0" "db 235" "db 236" "db 106" "db 1" "db 82" "db 232" "db 5" "db 0" "db 0" "db 0" "db 131" "db 196" "db 8" "db 93" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux fclose modify [eax ebx ecx edx esi edi ebp];
void fclose(void) {
    __db_FUN_0003b99e_0();
}
