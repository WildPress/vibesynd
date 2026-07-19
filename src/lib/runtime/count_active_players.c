/* @ 0x22c58 (80B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00022c58_0(void);
#pragma aux __db_FUN_00022c58_0 = "db 83" "db 86" "db 102" "db 139" "db 53" "db 12" "db 11" "db 1" "db 0" "db 49" "db 219" "db 49" "db 201" "db 235" "db 33" "db 141" "db 4" "db 149" "db 0" "db 0" "db 0" "db 0" "db 41" "db 208" "db 15" "db 181" "db 4" "db 69" "db 68" "db 6" "db 1" "db 0" "db 101" "db 128" "db 120" "db 2" "db 0" "db 116" "db 8" "db 101" "db 128" "db 120" "db 49" "db 255" "db 116" "db 1" "db 65" "db 67" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00022c58_1(void);
#pragma aux __db_FUN_00022c58_1 = "db 49" "db 210" "db 15" "db 191" "db 198" "db 102" "db 137" "db 218" "db 57" "db 194" "db 124" "db 211" "db 137" "db 200" "db 102" "db 137" "db 53" "db 12" "db 11" "db 1" "db 0" "db 94" "db 91" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 139" "db 210" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux count_active_players modify [eax ebx ecx edx esi edi ebp] aborts;
void count_active_players(void) {
    __db_FUN_00022c58_0();
    __db_FUN_00022c58_1();
}
