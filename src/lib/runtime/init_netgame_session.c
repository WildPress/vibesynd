/* @ 0x22e38 (160B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00022e38_0(void);
#pragma aux __db_FUN_00022e38_0 = "db 83" "db 185" "db 1" "db 0" "db 0" "db 0" "db 49" "db 210" "db 138" "db 37" "db 69" "db 11" "db 1" "db 0" "db 102" "db 137" "db 21" "db 22" "db 11" "db 1" "db 0" "db 102" "db 137" "db 13" "db 12" "db 11" "db 1" "db 0" "db 128" "db 252" "db 1" "db 117" "db 101" "db 232" "db 26" "db 80" "db 0" "db 0" "db 232" "db 197" "db 69" "db 0" "db 0" "db 102" "db 61" "db 254" "db 255" "db 114" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00022e38_1(void);
#pragma aux __db_FUN_00022e38_1 = "db 38" "db 118" "db 13" "db 102" "db 61" "db 255" "db 255" "db 117" "db 30" "db 104" "db 72" "db 2" "db 0" "db 0" "db 235" "db 5" "db 104" "db 92" "db 2" "db 0" "db 0" "db 232" "db 228" "db 126" "db 1" "db 0" "db 131" "db 196" "db 4" "db 106" "db 1" "db 232" "db 253" "db 126" "db 1" "db 0" "db 131" "db 196" "db 4" "db 49" "db 192" "db 80" "db 49" "db 192" "db 102" "db 161" "db 22" "db 11" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00022e38_2(void);
#pragma aux __db_FUN_00022e38_2 = "db 1" "db 0" "db 80" "db 232" "db 56" "db 0" "db 0" "db 0" "db 131" "db 196" "db 8" "db 232" "db 80" "db 75" "db 0" "db 0" "db 15" "db 191" "db 5" "db 22" "db 11" "db 1" "db 0" "db 80" "db 49" "db 219" "db 232" "db 1" "db 68" "db 0" "db 0" "db 131" "db 196" "db 4" "db 137" "db 216" "db 91" "db 195" "db 106" "db 0" "db 48" "db 210" "db 106" "db 0" "db 136" "db 21" "db 69" "db 11" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00022e38_3(void);
#pragma aux __db_FUN_00022e38_3 = "db 1" "db 0" "db 232" "db 9" "db 0" "db 0" "db 0" "db 131" "db 196" "db 8" "db 137" "db 216" "db 91" "db 195" "db 139" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux init_netgame_session modify [eax ebx ecx edx esi edi ebp] aborts;
void init_netgame_session(void) {
    __db_FUN_00022e38_0();
    __db_FUN_00022e38_1();
    __db_FUN_00022e38_2();
    __db_FUN_00022e38_3();
}
