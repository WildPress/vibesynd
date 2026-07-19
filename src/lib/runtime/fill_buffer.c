/* @ 0x3d40f (171B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d40f_0(void);
#pragma aux __db_FUN_0003d40f_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 93" "db 12" "db 131" "db 123" "db 8" "db 0" "db 117" "db 9" "db 83" "db 232" "db 114" "db 4" "db 0" "db 0" "db 131" "db 196" "db 4" "db 246" "db 67" "db 13" "db 32" "db 116" "db 19" "db 246" "db 67" "db 13" "db 6" "db 116" "db 13" "db 104" "db 0" "db 32" "db 0" "db 0" "db 232" "db 133" "db 16" "db 0" "db 0" "db 131" "db 196" "db 4" "db 128" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d40f_1(void);
#pragma aux __db_FUN_0003d40f_1 = "db 99" "db 12" "db 251" "db 139" "db 67" "db 8" "db 137" "db 3" "db 139" "db 67" "db 12" "db 37" "db 0" "db 36" "db 0" "db 0" "db 61" "db 0" "db 36" "db 0" "db 0" "db 117" "db 36" "db 131" "db 123" "db 16" "db 0" "db 117" "db 30" "db 199" "db 67" "db 4" "db 0" "db 0" "db 0" "db 0" "db 232" "db 144" "db 16" "db 0" "db 0" "db 131" "db 248" "db 255" "db 116" "db 46" "db 139" "db 19" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d40f_2(void);
#pragma aux __db_FUN_0003d40f_2 = "db 136" "db 2" "db 199" "db 67" "db 4" "db 1" "db 0" "db 0" "db 0" "db 235" "db 33" "db 246" "db 67" "db 13" "db 4" "db 116" "db 7" "db 184" "db 1" "db 0" "db 0" "db 0" "db 235" "db 3" "db 139" "db 67" "db 20" "db 80" "db 255" "db 51" "db 255" "db 115" "db 16" "db 232" "db 160" "db 4" "db 0" "db 0" "db 131" "db 196" "db 12" "db 137" "db 67" "db 4" "db 131" "db 123" "db 4" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d40f_3(void);
#pragma aux __db_FUN_0003d40f_3 = "db 127" "db 19" "db 117" "db 6" "db 128" "db 75" "db 12" "db 16" "db 235" "db 11" "db 128" "db 75" "db 12" "db 32" "db 199" "db 67" "db 4" "db 0" "db 0" "db 0" "db 0" "db 139" "db 67" "db 4" "db 93" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux fill_buffer modify [eax ebx ecx edx esi edi ebp];
void fill_buffer(void) { __db_FUN_0003d40f_0(); __db_FUN_0003d40f_1(); __db_FUN_0003d40f_2(); __db_FUN_0003d40f_3(); }
