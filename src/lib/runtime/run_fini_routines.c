/* @ 0x3d079 (67B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d079_0(void);
#pragma aux __db_FUN_0003d079_0 = "db 86" "db 87" "db 83" "db 6" "db 190" "db 68" "db 195" "db 0" "db 0" "db 191" "db 74" "db 195" "db 0" "db 0" "db 139" "db 223" "db 176" "db 0" "db 59" "db 247" "db 115" "db 20" "db 128" "db 62" "db 2" "db 116" "db 10" "db 58" "db 70" "db 1" "db 119" "db 5" "db 139" "db 222" "db 138" "db 70" "db 1" "db 131" "db 198" "db 6" "db 235" "db 232" "db 59" "db 223" "db 116" "db 16" "db 139" "db 67" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d079_1(void);
#pragma aux __db_FUN_0003d079_1 = "db 2" "db 11" "db 192" "db 116" "db 4" "db 30" "db 7" "db 255" "db 208" "db 198" "db 3" "db 2" "db 235" "db 198" "db 7" "db 91" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux run_fini_routines modify [eax ebx ecx edx esi edi ebp];
void run_fini_routines(void) { __db_FUN_0003d079_0(); __db_FUN_0003d079_1(); }
