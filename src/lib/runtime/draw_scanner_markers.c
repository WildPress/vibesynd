/* @ 0x1b658 (320B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0001b658_0(void);
#pragma aux __db_FUN_0001b658_0 = "db 83" "db 86" "db 87" "db 85" "db 128" "db 61" "db 79" "db 11" "db 1" "db 0" "db 0" "db 15" "db 132" "db 218" "db 0" "db 0" "db 0" "db 184" "db 25" "db 0" "db 0" "db 0" "db 139" "db 21" "db 180" "db 10" "db 1" "db 0" "db 41" "db 208" "db 139" "db 29" "db 184" "db 10" "db 1" "db 0" "db 80" "db 184" "db 16" "db 0" "db 0" "db 0" "db 41" "db 216" "db 80" "db 106" "db 0" "db 106" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_1(void);
#pragma aux __db_FUN_0001b658_1 = "db 4" "db 82" "db 83" "db 232" "db 36" "db 26" "db 3" "db 0" "db 184" "db 25" "db 0" "db 0" "db 0" "db 139" "db 61" "db 180" "db 10" "db 1" "db 0" "db 139" "db 45" "db 184" "db 10" "db 1" "db 0" "db 131" "db 196" "db 24" "db 41" "db 248" "db 133" "db 237" "db 117" "db 33" "db 133" "db 255" "db 15" "db 132" "db 215" "db 0" "db 0" "db 0" "db 87" "db 186" "db 16" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_2(void);
#pragma aux __db_FUN_0001b658_2 = "db 82" "db 80" "db 106" "db 4" "db 85" "db 85" "db 232" "db 241" "db 25" "db 3" "db 0" "db 131" "db 196" "db 24" "db 93" "db 95" "db 94" "db 91" "db 195" "db 80" "db 85" "db 184" "db 20" "db 0" "db 0" "db 0" "db 106" "db 0" "db 41" "db 232" "db 80" "db 87" "db 106" "db 0" "db 232" "db 213" "db 25" "db 3" "db 0" "db 139" "db 13" "db 180" "db 10" "db 1" "db 0" "db 131" "db 196" "db 24" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_3(void);
#pragma aux __db_FUN_0001b658_3 = "db 133" "db 201" "db 15" "db 132" "db 153" "db 0" "db 0" "db 0" "db 184" "db 16" "db 0" "db 0" "db 0" "db 139" "db 61" "db 184" "db 10" "db 1" "db 0" "db 81" "db 41" "db 248" "db 80" "db 184" "db 25" "db 0" "db 0" "db 0" "db 41" "db 200" "db 80" "db 106" "db 4" "db 106" "db 0" "db 87" "db 232" "db 163" "db 25" "db 3" "db 0" "db 131" "db 196" "db 24" "db 139" "db 21" "db 180" "db 10" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_4(void);
#pragma aux __db_FUN_0001b658_4 = "db 1" "db 0" "db 82" "db 139" "db 29" "db 184" "db 10" "db 1" "db 0" "db 184" "db 25" "db 0" "db 0" "db 0" "db 83" "db 41" "db 208" "db 80" "db 184" "db 20" "db 0" "db 0" "db 0" "db 41" "db 216" "db 80" "db 106" "db 0" "db 106" "db 0" "db 232" "db 121" "db 25" "db 3" "db 0" "db 131" "db 196" "db 24" "db 93" "db 95" "db 94" "db 91" "db 195" "db 49" "db 246" "db 141" "db 64" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_5(void);
#pragma aux __db_FUN_0001b658_5 = "db 49" "db 219" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 146" "db 0" "db 0" "db 0" "db 0" "db 139" "db 192" "db 15" "db 191" "db 198" "db 15" "db 191" "db 211" "db 193" "db 224" "db 4" "db 128" "db 188" "db 2" "db 44" "db 219" "db 0" "db 0" "db 0" "db 116" "db 16" "db 15" "db 191" "db 198" "db 80" "db 15" "db 191" "db 195" "db 80" "db 232" "db 239" "db 227" "db 2" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001b658_6(void);
#pragma aux __db_FUN_0001b658_6 = "db 131" "db 196" "db 8" "db 67" "db 102" "db 131" "db 251" "db 16" "db 124" "db 214" "db 70" "db 102" "db 131" "db 254" "db 25" "db 124" "db 191" "db 93" "db 95" "db 94" "db 91" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 84" "db 34" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux draw_scanner_markers modify [eax ebx ecx edx esi edi ebp] aborts;
void draw_scanner_markers(void) {
    __db_FUN_0001b658_0();
    __db_FUN_0001b658_1();
    __db_FUN_0001b658_2();
    __db_FUN_0001b658_3();
    __db_FUN_0001b658_4();
    __db_FUN_0001b658_5();
    __db_FUN_0001b658_6();
}
