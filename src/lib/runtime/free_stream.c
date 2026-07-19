/* @ 0x3dae1 (51B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003dae1_0(void);
#pragma aux __db_FUN_0003dae1_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 93" "db 12" "db 186" "db 76" "db 30" "db 1" "db 0" "db 139" "db 2" "db 133" "db 192" "db 116" "db 30" "db 59" "db 88" "db 4" "db 116" "db 4" "db 137" "db 194" "db 235" "db 241" "db 128" "db 75" "db 12" "db 3" "db 139" "db 24" "db 137" "db 26" "db 139" "db 21" "db 68" "db 30" "db 1" "db 0" "db 137" "db 16" "db 163" "db 68" "db 30" "db 1" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003dae1_1(void);
#pragma aux __db_FUN_0003dae1_1 = "db 93" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux free_stream modify [eax ebx ecx edx esi edi ebp];
void free_stream(void) { __db_FUN_0003dae1_0(); __db_FUN_0003dae1_1(); }
