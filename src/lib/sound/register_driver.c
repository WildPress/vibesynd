/* @ 0x398d7 (143B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000398d7_0(void);
#pragma aux __db_FUN_000398d7_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 199" "db 5" "db 190" "db 189" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 139" "db 53" "db 190" "db 189" "db 0" "db 0" "db 209" "db 230" "db 209" "db 230" "db 139" "db 134" "db 250" "db 188" "db 0" "db 0" "db 131" "db 248" "db 0" "db 116" "db 22" "db 255" "db 5" "db 190" "db 189" "db 0" "db 0" "db 131" "db 61" "db 190" "db 189" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_000398d7_1(void);
#pragma aux __db_FUN_000398d7_1 = "db 0" "db 0" "db 16" "db 117" "db 220" "db 184" "db 255" "db 255" "db 255" "db 255" "db 235" "db 66" "db 139" "db 125" "db 8" "db 184" "db 255" "db 255" "db 255" "db 255" "db 129" "db 127" "db 4" "db 67" "db 111" "db 112" "db 121" "db 117" "db 49" "db 139" "db 63" "db 137" "db 190" "db 250" "db 188" "db 0" "db 0" "db 255" "db 53" "db 190" "db 189" "db 0" "db 0" "db 232" "db 93" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_000398d7_2(void);
#pragma aux __db_FUN_000398d7_2 = "db 131" "db 196" "db 4" "db 139" "db 248" "db 131" "db 248" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 116" "db 15" "db 139" "db 23" "db 59" "db 21" "db 204" "db 189" "db 0" "db 0" "db 119" "db 5" "db 161" "db 190" "db 189" "db 0" "db 0" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux register_driver modify [eax ebx ecx edx esi edi ebp];
void register_driver(void) {
    __db_FUN_000398d7_0();
    __db_FUN_000398d7_1();
    __db_FUN_000398d7_2();
}
