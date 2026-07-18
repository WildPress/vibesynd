/* @ 0x4a574 (52B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004a574_0(void);
#pragma aux __db_FUN_0004a574_0 = "db 83" "db 81" "db 82" "db 86" "db 87" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 2" "db 116" "db 7" "db 232" "db 33" "db 0" "db 0" "db 0" "db 235" "db 25" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 4" "db 116" "db 7" "db 232" "db 162" "db 0" "db 0" "db 0" "db 235" "db 9" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 1" "db 116" "db 0" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004a574_1(void);
#pragma aux __db_FUN_0004a574_1 = "db 90" "db 89" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux present_frame_dispatch modify [eax ebx ecx edx esi edi ebp];
void present_frame_dispatch(void) { __db_FUN_0004a574_0(); __db_FUN_0004a574_1(); }
