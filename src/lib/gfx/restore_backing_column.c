/* @ 0x4a3cc (99B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004a3cc_0(void);
#pragma aux __db_FUN_0004a3cc_0 = "db 85" "db 139" "db 236" "db 80" "db 83" "db 81" "db 82" "db 86" "db 87" "db 139" "db 53" "db 104" "db 83" "db 0" "db 0" "db 191" "db 116" "db 203" "db 3" "db 0" "db 15" "db 183" "db 69" "db 8" "db 107" "db 192" "db 80" "db 3" "db 240" "db 185" "db 17" "db 0" "db 0" "db 0" "db 102" "db 129" "db 125" "db 8" "db 144" "db 1" "db 125" "db 49" "db 139" "db 23" "db 137" "db 22" "db 131" "db 199" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004a3cc_1(void);
#pragma aux __db_FUN_0004a3cc_1 = "db 4" "db 139" "db 23" "db 137" "db 150" "db 0" "db 125" "db 0" "db 0" "db 131" "db 199" "db 4" "db 139" "db 23" "db 137" "db 150" "db 0" "db 250" "db 0" "db 0" "db 131" "db 199" "db 4" "db 139" "db 23" "db 137" "db 150" "db 0" "db 119" "db 1" "db 0" "db 131" "db 199" "db 4" "db 131" "db 198" "db 80" "db 102" "db 255" "db 69" "db 8" "db 226" "db 199" "db 95" "db 94" "db 90" "db 89" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004a3cc_2(void);
#pragma aux __db_FUN_0004a3cc_2 = "db 88" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux restore_backing_column modify [eax ebx ecx edx esi edi ebp];
void restore_backing_column(void) { __db_FUN_0004a3cc_0(); __db_FUN_0004a3cc_1(); __db_FUN_0004a3cc_2(); }
