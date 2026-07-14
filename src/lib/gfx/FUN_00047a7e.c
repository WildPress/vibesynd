/* @ 0x47a7e (140B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00047a7e_0(void);
#pragma aux __db_FUN_00047a7e_0 = "db 85" "db 139" "db 236" "db 83" "db 81" "db 82" "db 86" "db 87" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 2" "db 15" "db 132" "db 207" "db 29" "db 0" "db 0" "db 102" "db 139" "db 69" "db 8" "db 102" "db 139" "db 93" "db 12" "db 102" "db 139" "db 200" "db 102" "db 139" "db 211" "db 102" "db 193" "db 248" "db 8" "db 102" "db 193" "db 251" "db 8" "db 102" "db 193" "db 227" "db 7" "db 102" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00047a7e_1(void);
#pragma aux __db_FUN_00047a7e_1 = "db 139" "db 240" "db 102" "db 3" "db 243" "db 129" "db 230" "db 255" "db 255" "db 0" "db 0" "db 193" "db 230" "db 2" "db 102" "db 129" "db 225" "db 255" "db 0" "db 102" "db 129" "db 226" "db 255" "db 0" "db 102" "db 139" "db 218" "db 102" "db 3" "db 217" "db 184" "db 0" "db 0" "db 0" "db 0" "db 102" "db 129" "db 251" "db 0" "db 1" "db 124" "db 3" "db 131" "db 192" "db 8" "db 102" "db 59" "db 202" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00047a7e_2(void);
#pragma aux __db_FUN_00047a7e_2 = "db 124" "db 3" "db 131" "db 192" "db 4" "db 3" "db 53" "db 88" "db 83" "db 0" "db 0" "db 255" "db 160" "db 167" "db 163" "db 3" "db 0" "db 197" "db 163" "db 3" "db 0" "db 119" "db 178" "db 3" "db 0" "db 183" "db 163" "db 3" "db 0" "db 194" "db 163" "db 3" "db 0" "db 129" "db 198" "db 0" "db 2" "db 0" "db 0" "db 233" "db 181" "db 14" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00047a7e modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_00047a7e(void) { __db_FUN_00047a7e_0(); __db_FUN_00047a7e_1(); __db_FUN_00047a7e_2(); }
