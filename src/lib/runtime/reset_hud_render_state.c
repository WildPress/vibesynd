/* @ 0x20ef8 (208B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00020ef8_0(void);
#pragma aux __db_FUN_00020ef8_0 = "db 83" "db 87" "db 85" "db 176" "db 1" "db 191" "db 7" "db 0" "db 0" "db 0" "db 49" "db 210" "db 48" "db 228" "db 49" "db 219" "db 49" "db 201" "db 137" "db 21" "db 240" "db 10" "db 1" "db 0" "db 136" "db 37" "db 244" "db 10" "db 1" "db 0" "db 136" "db 37" "db 245" "db 10" "db 1" "db 0" "db 136" "db 37" "db 246" "db 10" "db 1" "db 0" "db 136" "db 37" "db 247" "db 10" "db 1" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00020ef8_1(void);
#pragma aux __db_FUN_00020ef8_1 = "db 136" "db 37" "db 248" "db 10" "db 1" "db 0" "db 136" "db 37" "db 249" "db 10" "db 1" "db 0" "db 136" "db 37" "db 250" "db 10" "db 1" "db 0" "db 162" "db 252" "db 10" "db 1" "db 0" "db 136" "db 37" "db 253" "db 10" "db 1" "db 0" "db 137" "db 29" "db 254" "db 10" "db 1" "db 0" "db 137" "db 29" "db 2" "db 11" "db 1" "db 0" "db 137" "db 29" "db 6" "db 11" "db 1" "db 0" "db 136" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00020ef8_2(void);
#pragma aux __db_FUN_00020ef8_2 = "db 37" "db 251" "db 10" "db 1" "db 0" "db 162" "db 127" "db 83" "db 0" "db 0" "db 162" "db 77" "db 11" "db 1" "db 0" "db 162" "db 75" "db 11" "db 1" "db 0" "db 137" "db 61" "db 4" "db 83" "db 0" "db 0" "db 102" "db 137" "db 13" "db 50" "db 11" "db 1" "db 0" "db 187" "db 16" "db 0" "db 0" "db 0" "db 136" "db 198" "db 48" "db 210" "db 48" "db 198" "db 102" "db 137" "db 29" "db 44" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00020ef8_3(void);
#pragma aux __db_FUN_00020ef8_3 = "db 83" "db 0" "db 0" "db 102" "db 137" "db 21" "db 46" "db 83" "db 0" "db 0" "db 48" "db 192" "db 49" "db 210" "db 102" "db 137" "db 194" "db 49" "db 237" "db 64" "db 137" "db 44" "db 149" "db 124" "db 10" "db 1" "db 0" "db 102" "db 61" "db 8" "db 0" "db 114" "db 235" "db 104" "db 8" "db 4" "db 0" "db 0" "db 85" "db 104" "db 116" "db 6" "db 1" "db 0" "db 137" "db 45" "db 232" "db 10" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00020ef8_4(void);
#pragma aux __db_FUN_00020ef8_4 = "db 1" "db 0" "db 232" "db 218" "db 193" "db 2" "db 0" "db 131" "db 196" "db 12" "db 93" "db 95" "db 91" "db 195" "db 139" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux reset_hud_render_state modify [eax ebx ecx edx esi edi ebp] aborts;
void reset_hud_render_state(void) {
    __db_FUN_00020ef8_0();
    __db_FUN_00020ef8_1();
    __db_FUN_00020ef8_2();
    __db_FUN_00020ef8_3();
    __db_FUN_00020ef8_4();
}
