/* @ 0x39bd7 (201B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039bd7_0(void);
#pragma aux __db_FUN_00039bd7_0 = "db 184" "db 174" "db 0" "db 0" "db 0" "db 233" "db 203" "db 246" "db 255" "db 255" "db 184" "db 175" "db 0" "db 0" "db 0" "db 233" "db 193" "db 246" "db 255" "db 255" "db 184" "db 176" "db 0" "db 0" "db 0" "db 233" "db 183" "db 246" "db 255" "db 255" "db 184" "db 177" "db 0" "db 0" "db 0" "db 233" "db 173" "db 246" "db 255" "db 255" "db 184" "db 178" "db 0" "db 0" "db 0" "db 233" "db 163" "db 246" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039bd7_1(void);
#pragma aux __db_FUN_00039bd7_1 = "db 255" "db 255" "db 184" "db 179" "db 0" "db 0" "db 0" "db 233" "db 153" "db 246" "db 255" "db 255" "db 184" "db 180" "db 0" "db 0" "db 0" "db 233" "db 143" "db 246" "db 255" "db 255" "db 184" "db 181" "db 0" "db 0" "db 0" "db 233" "db 133" "db 246" "db 255" "db 255" "db 184" "db 182" "db 0" "db 0" "db 0" "db 233" "db 123" "db 246" "db 255" "db 255" "db 184" "db 183" "db 0" "db 0" "db 0" "db 233" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039bd7_2(void);
#pragma aux __db_FUN_00039bd7_2 = "db 113" "db 246" "db 255" "db 255" "db 184" "db 185" "db 0" "db 0" "db 0" "db 233" "db 103" "db 246" "db 255" "db 255" "db 184" "db 186" "db 0" "db 0" "db 0" "db 233" "db 93" "db 246" "db 255" "db 255" "db 184" "db 187" "db 0" "db 0" "db 0" "db 233" "db 83" "db 246" "db 255" "db 255" "db 184" "db 188" "db 0" "db 0" "db 0" "db 233" "db 73" "db 246" "db 255" "db 255" "db 184" "db 189" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039bd7_3(void);
#pragma aux __db_FUN_00039bd7_3 = "db 0" "db 233" "db 63" "db 246" "db 255" "db 255" "db 184" "db 190" "db 0" "db 0" "db 0" "db 233" "db 53" "db 246" "db 255" "db 255" "db 184" "db 191" "db 0" "db 0" "db 0" "db 233" "db 43" "db 246" "db 255" "db 255" "db 184" "db 192" "db 0" "db 0" "db 0" "db 233" "db 33" "db 246" "db 255" "db 255" "db 184" "db 193" "db 0" "db 0" "db 0" "db 233" "db 23" "db 246" "db 255" "db 255" "db 184" "db 194" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039bd7_4(void);
#pragma aux __db_FUN_00039bd7_4 = "db 0" "db 0" "db 0" "db 233" "db 13" "db 246" "db 255" "db 255" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux sound_cmd_thunks modify [eax ebx ecx edx esi edi ebp] aborts;
void sound_cmd_thunks(void) {
    __db_FUN_00039bd7_0();
    __db_FUN_00039bd7_1();
    __db_FUN_00039bd7_2();
    __db_FUN_00039bd7_3();
    __db_FUN_00039bd7_4();
}
