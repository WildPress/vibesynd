/* @ 0x4107b (95B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004107b_0(void);
#pragma aux __db_FUN_0004107b_0 = "db 85" "db 139" "db 236" "db 102" "db 139" "db 69" "db 8" "db 102" "db 139" "db 93" "db 12" "db 102" "db 139" "db 200" "db 102" "db 139" "db 211" "db 102" "db 193" "db 248" "db 8" "db 102" "db 193" "db 251" "db 8" "db 102" "db 193" "db 227" "db 7" "db 102" "db 139" "db 240" "db 102" "db 3" "db 243" "db 129" "db 230" "db 255" "db 255" "db 0" "db 0" "db 193" "db 230" "db 2" "db 102" "db 129" "db 225" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004107b_1(void);
#pragma aux __db_FUN_0004107b_1 = "db 0" "db 102" "db 129" "db 226" "db 255" "db 0" "db 102" "db 139" "db 218" "db 102" "db 3" "db 217" "db 184" "db 0" "db 0" "db 0" "db 0" "db 102" "db 129" "db 251" "db 0" "db 1" "db 124" "db 3" "db 131" "db 192" "db 8" "db 102" "db 59" "db 202" "db 124" "db 3" "db 131" "db 192" "db 4" "db 3" "db 53" "db 88" "db 83" "db 0" "db 0" "db 255" "db 160" "db 146" "db 57" "db 3" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux iso_tile_dispatch modify [eax ebx ecx edx esi edi ebp] aborts;
void iso_tile_dispatch(void) { __db_FUN_0004107b_0(); __db_FUN_0004107b_1(); }
