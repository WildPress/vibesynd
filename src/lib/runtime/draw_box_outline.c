/* @ 0x15ee8 (112B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00015ee8_0(void);
#pragma aux __db_FUN_00015ee8_0 = "db 83" "db 86" "db 87" "db 85" "db 131" "db 236" "db 4" "db 15" "db 182" "db 116" "db 36" "db 40" "db 86" "db 15" "db 191" "db 124" "db 36" "db 32" "db 87" "db 15" "db 191" "db 92" "db 36" "db 40" "db 83" "db 87" "db 15" "db 191" "db 108" "db 36" "db 40" "db 85" "db 137" "db 108" "db 36" "db 20" "db 232" "db 215" "db 43" "db 0" "db 0" "db 131" "db 196" "db 20" "db 86" "db 15" "db 191" "db 108" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00015ee8_1(void);
#pragma aux __db_FUN_00015ee8_1 = "db 36" "db 40" "db 85" "db 83" "db 87" "db 83" "db 232" "db 197" "db 43" "db 0" "db 0" "db 131" "db 196" "db 20" "db 86" "db 85" "db 139" "db 76" "db 36" "db 8" "db 81" "db 85" "db 83" "db 232" "db 180" "db 43" "db 0" "db 0" "db 131" "db 196" "db 20" "db 86" "db 87" "db 139" "db 116" "db 36" "db 8" "db 86" "db 85" "db 86" "db 232" "db 163" "db 43" "db 0" "db 0" "db 131" "db 196" "db 20" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00015ee8_2(void);
#pragma aux __db_FUN_00015ee8_2 = "db 131" "db 196" "db 4" "db 93" "db 95" "db 94" "db 91" "db 195" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux draw_box_outline modify [eax ebx ecx edx esi edi ebp] aborts;
void draw_box_outline(void) {
    __db_FUN_00015ee8_0();
    __db_FUN_00015ee8_1();
    __db_FUN_00015ee8_2();
}
