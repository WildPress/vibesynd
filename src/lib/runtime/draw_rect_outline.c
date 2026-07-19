/* @ 0x18a28 (144B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00018a28_0(void);
#pragma aux __db_FUN_00018a28_0 = "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 146" "db 0" "db 0" "db 0" "db 0" "db 141" "db 64" "db 0" "db 83" "db 86" "db 87" "db 85" "db 131" "db 236" "db 4" "db 139" "db 68" "db 36" "db 36" "db 15" "db 191" "db 116" "db 36" "db 40" "db 15" "db 191" "db 92" "db 36" "db 24" "db 15" "db 191" "db 124" "db 36" "db 28" "db 72" "db 86" "db 15" "db 191" "db 232" "db 83" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00018a28_1(void);
#pragma aux __db_FUN_00018a28_1 = "db 141" "db 4" "db 47" "db 139" "db 84" "db 36" "db 40" "db 80" "db 74" "db 87" "db 137" "db 84" "db 36" "db 48" "db 137" "db 68" "db 36" "db 16" "db 232" "db 69" "db 106" "db 2" "db 0" "db 131" "db 196" "db 16" "db 15" "db 191" "db 108" "db 36" "db 32" "db 86" "db 1" "db 221" "db 85" "db 139" "db 76" "db 36" "db 8" "db 81" "db 87" "db 232" "db 46" "db 106" "db 2" "db 0" "db 131" "db 196" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00018a28_2(void);
#pragma aux __db_FUN_00018a28_2 = "db 16" "db 86" "db 87" "db 85" "db 83" "db 232" "db 164" "db 107" "db 2" "db 0" "db 131" "db 196" "db 16" "db 86" "db 139" "db 116" "db 36" "db 4" "db 86" "db 85" "db 83" "db 232" "db 148" "db 107" "db 2" "db 0" "db 131" "db 196" "db 16" "db 131" "db 196" "db 4" "db 93" "db 95" "db 94" "db 91" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 82" "db 0" "db 139" "db 219" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux draw_rect_outline modify [eax ebx ecx edx esi edi ebp] aborts;
void draw_rect_outline(void) {
    __db_FUN_00018a28_0();
    __db_FUN_00018a28_1();
    __db_FUN_00018a28_2();
}
