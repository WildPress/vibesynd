/* @ 0x3e513 (125B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003e513_0(void);
#pragma aux __db_FUN_0003e513_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 125" "db 20" "db 87" "db 232" "db 7" "db 224" "db 255" "db 255" "db 131" "db 196" "db 4" "db 168" "db 128" "db 116" "db 48" "db 176" "db 2" "db 102" "db 137" "db 251" "db 49" "db 210" "db 180" "db 66" "db 139" "db 202" "db 193" "db 233" "db 16" "db 205" "db 33" "db 102" "db 209" "db 210" "db 102" "db 209" "db 202" "db 193" "db 226" "db 16" "db 102" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003e513_1(void);
#pragma aux __db_FUN_0003e513_1 = "db 139" "db 208" "db 137" "db 214" "db 133" "db 210" "db 125" "db 14" "db 15" "db 183" "db 194" "db 80" "db 232" "db 101" "db 223" "db 255" "db 255" "db 131" "db 196" "db 4" "db 235" "db 50" "db 139" "db 85" "db 24" "db 139" "db 77" "db 28" "db 102" "db 137" "db 251" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 194" "db 137" "db 198" "db 137" "db 199" "db 133" "db 192" "db 125" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003e513_2(void);
#pragma aux __db_FUN_0003e513_2 = "db 5" "db 15" "db 183" "db 192" "db 235" "db 213" "db 59" "db 69" "db 28" "db 116" "db 11" "db 232" "db 234" "db 222" "db 255" "db 255" "db 199" "db 0" "db 12" "db 0" "db 0" "db 0" "db 137" "db 248" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux qwrite modify [eax ebx ecx edx esi edi ebp];
void qwrite(void) { __db_FUN_0003e513_0(); __db_FUN_0003e513_1(); __db_FUN_0003e513_2(); }
