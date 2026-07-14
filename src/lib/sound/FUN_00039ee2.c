/* @ 0x39ee2 (71B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039ee2_0(void);
#pragma aux __db_FUN_00039ee2_0 = "db 15" "db 183" "db 5" "db 220" "db 189" "db 0" "db 0" "db 131" "db 232" "db 6" "db 80" "db 255" "db 53" "db 112" "db 83" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "db 232" "db 198" "db 8" "db 0" "db 0" "db 131" "db 196" "db 12" "db 139" "db 61" "db 112" "db 83" "db 0" "db 0" "db 102" "db 139" "db 7" "db 102" "db 163" "db 214" "db 189" "db 0" "db 0" "db 131" "db 199" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039ee2_1(void);
#pragma aux __db_FUN_00039ee2_1 = "db 2" "db 102" "db 139" "db 7" "db 102" "db 163" "db 216" "db 189" "db 0" "db 0" "db 131" "db 199" "db 2" "db 102" "db 139" "db 7" "db 102" "db 163" "db 218" "db 189" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00039ee2 modify [eax ebx ecx edx esi edi ebp];
void FUN_00039ee2(void) {
    __db_FUN_00039ee2_0();
    __db_FUN_00039ee2_1();
}
