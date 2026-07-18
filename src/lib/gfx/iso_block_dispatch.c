/* @ 0x45e61 (151B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00045e61_0(void);
#pragma aux __db_FUN_00045e61_0 = "db 85" "db 139" "db 236" "db 80" "db 83" "db 81" "db 82" "db 86" "db 87" "db 102" "db 139" "db 69" "db 8" "db 102" "db 139" "db 93" "db 12" "db 102" "db 139" "db 200" "db 102" "db 139" "db 211" "db 102" "db 209" "db 248" "db 102" "db 209" "db 251" "db 102" "db 193" "db 227" "db 7" "db 102" "db 139" "db 240" "db 102" "db 3" "db 243" "db 129" "db 230" "db 255" "db 255" "db 0" "db 0" "db 193" "db 230" "db 2" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00045e61_1(void);
#pragma aux __db_FUN_00045e61_1 = "db 102" "db 131" "db 225" "db 1" "db 102" "db 131" "db 226" "db 1" "db 102" "db 139" "db 218" "db 102" "db 3" "db 217" "db 184" "db 0" "db 0" "db 0" "db 0" "db 102" "db 131" "db 251" "db 2" "db 124" "db 3" "db 131" "db 192" "db 8" "db 102" "db 59" "db 202" "db 124" "db 3" "db 131" "db 192" "db 4" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 2" "db 15" "db 132" "db 172" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00045e61_2(void);
#pragma aux __db_FUN_00045e61_2 = "db 0" "db 105" "db 93" "db 24" "db 0" "db 5" "db 0" "db 0" "db 137" "db 93" "db 24" "db 193" "db 101" "db 20" "db 2" "db 3" "db 53" "db 88" "db 83" "db 0" "db 0" "db 255" "db 160" "db 148" "db 135" "db 3" "db 0" "db 177" "db 135" "db 3" "db 0" "db 236" "db 135" "db 3" "db 0" "db 164" "db 135" "db 3" "db 0" "db 172" "db 135" "db 3" "db 0" "db 129" "db 198" "db 0" "db 2" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00045e61_3(void);
#pragma aux __db_FUN_00045e61_3 = "db 0" "db 235" "db 64" "db 131" "db 198" "db 4" "db 235" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux iso_block_dispatch modify [eax ebx ecx edx esi edi ebp] aborts;
void iso_block_dispatch(void) { __db_FUN_00045e61_0(); __db_FUN_00045e61_1(); __db_FUN_00045e61_2(); __db_FUN_00045e61_3(); }
