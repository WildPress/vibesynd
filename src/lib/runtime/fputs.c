/* @ 0x3d24a (121B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d24a_0(void);
#pragma aux __db_FUN_0003d24a_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 93" "db 20" "db 139" "db 117" "db 24" "db 131" "db 126" "db 8" "db 0" "db 117" "db 9" "db 86" "db 232" "db 47" "db 6" "db 0" "db 0" "db 131" "db 196" "db 4" "db 199" "db 69" "db 252" "db 0" "db 0" "db 0" "db 0" "db 246" "db 70" "db 13" "db 4" "db 116" "db 15" "db 128" "db 102" "db 13" "db 251" "db 199" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d24a_1(void);
#pragma aux __db_FUN_0003d24a_1 = "db 69" "db 252" "db 1" "db 0" "db 0" "db 0" "db 128" "db 78" "db 13" "db 2" "db 49" "db 255" "db 15" "db 182" "db 3" "db 67" "db 133" "db 192" "db 116" "db 17" "db 86" "db 80" "db 232" "db 46" "db 0" "db 0" "db 0" "db 131" "db 196" "db 8" "db 131" "db 248" "db 255" "db 117" "db 233" "db 137" "db 199" "db 131" "db 125" "db 252" "db 0" "db 116" "db 23" "db 128" "db 102" "db 13" "db 253" "db 128" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003d24a_2(void);
#pragma aux __db_FUN_0003d24a_2 = "db 78" "db 13" "db 4" "db 133" "db 255" "db 117" "db 11" "db 86" "db 232" "db 175" "db 6" "db 0" "db 0" "db 131" "db 196" "db 4" "db 137" "db 199" "db 137" "db 248" "db 201" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux fputs modify [eax ebx ecx edx esi edi ebp];
void fputs(void) { __db_FUN_0003d24a_0(); __db_FUN_0003d24a_1(); __db_FUN_0003d24a_2(); }
