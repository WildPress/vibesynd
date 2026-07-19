/* @ 0x3dcfa (157B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003dcfa_0(void);
#pragma aux __db_FUN_0003dcfa_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 69" "db 20" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 1" "db 116" "db 13" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 9" "db 15" "db 133" "db 106" "db 0" "db 0" "db 0" "db 133" "db 192" "db 126" "db 84" "db 5" "db 255" "db 15" "db 0" "db 0" "db 102" "db 37" "db 0" "db 240" "db 128" "db 61" "db 218" "db 194" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003dcfa_1(void);
#pragma aux __db_FUN_0003dcfa_1 = "db 0" "db 0" "db 1" "db 117" "db 34" "db 137" "db 194" "db 193" "db 250" "db 16" "db 102" "db 137" "db 193" "db 102" "db 137" "db 211" "db 180" "db 5" "db 176" "db 1" "db 205" "db 49" "db 25" "db 192" "db 247" "db 208" "db 117" "db 9" "db 102" "db 137" "db 216" "db 193" "db 224" "db 16" "db 102" "db 137" "db 200" "db 235" "db 15" "db 137" "db 195" "db 184" "db 0" "db 72" "db 0" "db 128" "db 205" "db 33" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003dcfa_2(void);
#pragma aux __db_FUN_0003dcfa_2 = "db 25" "db 219" "db 247" "db 211" "db 33" "db 216" "db 133" "db 192" "db 117" "db 46" "db 232" "db 4" "db 231" "db 255" "db 255" "db 199" "db 0" "db 5" "db 0" "db 0" "db 0" "db 235" "db 11" "db 232" "db 247" "db 230" "db 255" "db 255" "db 199" "db 0" "db 9" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 235" "db 15" "db 3" "db 5" "db 172" "db 194" "db 0" "db 0" "db 80" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003dcfa_3(void);
#pragma aux __db_FUN_0003dcfa_3 = "db 232" "db 8" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux os_getmem modify [eax ebx ecx edx esi edi ebp];
void os_getmem(void) { __db_FUN_0003dcfa_0(); __db_FUN_0003dcfa_1(); __db_FUN_0003dcfa_2(); __db_FUN_0003dcfa_3(); }
