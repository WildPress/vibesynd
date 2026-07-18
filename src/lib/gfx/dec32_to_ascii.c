/* @ 0x4d451 (39B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004d451_0(void);
#pragma aux __db_FUN_0004d451_0 = "db 51" "db 219" "db 102" "db 185" "db 10" "db 0" "db 50" "db 210" "db 43" "db 131" "db 48" "db 253" "db 3" "db 0" "db 114" "db 4" "db 254" "db 194" "db 235" "db 244" "db 3" "db 131" "db 48" "db 253" "db 3" "db 0" "db 128" "db 194" "db 48" "db 136" "db 23" "db 71" "db 131" "db 195" "db 4" "db 103" "db 226" "db 224" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux dec32_to_ascii modify [eax ebx ecx edx esi edi ebp];
void dec32_to_ascii(void) { __db_FUN_0004d451_0(); }
