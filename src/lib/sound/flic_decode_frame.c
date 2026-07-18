/* @ 0x39e42 (160B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039e42_0(void);
#pragma aux __db_FUN_00039e42_0 = "db 106" "db 2" "db 104" "db 224" "db 189" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "db 232" "db 112" "db 9" "db 0" "db 0" "db 131" "db 196" "db 12" "db 106" "db 8" "db 255" "db 53" "db 112" "db 83" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "db 232" "db 90" "db 9" "db 0" "db 0" "db 131" "db 196" "db 12" "db 102" "db 161" "db 224" "db 189" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039e42_1(void);
#pragma aux __db_FUN_00039e42_1 = "db 0" "db 102" "db 131" "db 248" "db 0" "db 117" "db 2" "db 235" "db 102" "db 102" "db 131" "db 45" "db 224" "db 189" "db 0" "db 0" "db 1" "db 106" "db 4" "db 104" "db 220" "db 189" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "db 232" "db 47" "db 9" "db 0" "db 0" "db 131" "db 196" "db 12" "db 106" "db 2" "db 104" "db 212" "db 189" "db 0" "db 0" "db 255" "db 53" "db 208" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039e42_2(void);
#pragma aux __db_FUN_00039e42_2 = "db 189" "db 0" "db 0" "db 232" "db 26" "db 9" "db 0" "db 0" "db 131" "db 196" "db 12" "db 102" "db 161" "db 212" "db 189" "db 0" "db 0" "db 102" "db 131" "db 248" "db 7" "db 117" "db 7" "db 232" "db 117" "db 1" "db 0" "db 0" "db 235" "db 173" "db 102" "db 131" "db 248" "db 4" "db 117" "db 7" "db 232" "db 158" "db 0" "db 0" "db 0" "db 235" "db 160" "db 102" "db 131" "db 248" "db 15" "db 117" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039e42_3(void);
#pragma aux __db_FUN_00039e42_3 = "db 7" "db 232" "db 52" "db 2" "db 0" "db 0" "db 235" "db 147" "db 232" "db 106" "db 0" "db 0" "db 0" "db 235" "db 140" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux flic_decode_frame modify [eax ebx ecx edx esi edi ebp];
void flic_decode_frame(void) {
    __db_FUN_00039e42_0();
    __db_FUN_00039e42_1();
    __db_FUN_00039e42_2();
    __db_FUN_00039e42_3();
}
