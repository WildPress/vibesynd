/* @ 0x3e698 (19B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003e698_0(void);
#pragma aux __db_FUN_0003e698_0 = "db 115" "db 14" "db 37" "db 255" "db 255" "db 0" "db 0" "db 80" "db 232" "db 20" "db 222" "db 255" "db 255" "db 88" "db 235" "db 2" "db 43" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux check_dos_carry modify [eax ebx ecx edx esi edi ebp];
void check_dos_carry(void) { __db_FUN_0003e698_0(); }
