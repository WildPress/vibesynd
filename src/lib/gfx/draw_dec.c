/* @ 0x4cb1a (78B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004cb1a_0(void);
#pragma aux __db_FUN_0004cb1a_0 = "db 85" "db 139" "db 236" "db 83" "db 81" "db 82" "db 87" "db 86" "db 139" "db 69" "db 8" "db 191" "db 132" "db 227" "db 0" "db 0" "db 232" "db 34" "db 9" "db 0" "db 0" "db 198" "db 7" "db 0" "db 15" "db 183" "db 69" "db 28" "db 247" "db 216" "db 5" "db 132" "db 227" "db 0" "db 0" "db 131" "db 192" "db 10" "db 131" "db 236" "db 2" "db 102" "db 255" "db 117" "db 24" "db 131" "db 236" "db 2" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004cb1a_1(void);
#pragma aux __db_FUN_0004cb1a_1 = "db 102" "db 255" "db 117" "db 20" "db 131" "db 236" "db 2" "db 102" "db 255" "db 117" "db 16" "db 255" "db 117" "db 12" "db 80" "db 232" "db 10" "db 0" "db 0" "db 0" "db 131" "db 196" "db 20" "db 94" "db 95" "db 90" "db 89" "db 91" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux draw_dec modify [eax ebx ecx edx esi edi ebp];
void draw_dec(void) { __db_FUN_0004cb1a_0(); __db_FUN_0004cb1a_1(); }
