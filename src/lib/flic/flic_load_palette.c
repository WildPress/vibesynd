/* @ 0x39f92 (161B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039f92_0(void);
#pragma aux __db_FUN_00039f92_0 = "db 86" "db 102" "db 199" "db 5" "db 46" "db 190" "db 0" "db 0" "db 0" "db 0" "db 131" "db 61" "db 244" "db 189" "db 0" "db 0" "db 1" "db 15" "db 133" "db 136" "db 0" "db 0" "db 0" "db 139" "db 61" "db 176" "db 10" "db 1" "db 0" "db 102" "db 43" "db 219" "db 102" "db 139" "db 7" "db 131" "db 199" "db 2" "db 139" "db 53" "db 168" "db 10" "db 1" "db 0" "db 102" "db 80" "db 185" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039f92_1(void);
#pragma aux __db_FUN_00039f92_1 = "db 0" "db 0" "db 0" "db 138" "db 15" "db 71" "db 3" "db 241" "db 3" "db 241" "db 3" "db 241" "db 102" "db 186" "db 0" "db 0" "db 138" "db 23" "db 71" "db 128" "db 250" "db 0" "db 117" "db 4" "db 102" "db 186" "db 0" "db 1" "db 138" "db 7" "db 71" "db 136" "db 6" "db 70" "db 138" "db 7" "db 71" "db 136" "db 6" "db 70" "db 138" "db 7" "db 71" "db 136" "db 6" "db 70" "db 102" "db 74" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039f92_2(void);
#pragma aux __db_FUN_00039f92_2 = "db 102" "db 131" "db 250" "db 0" "db 117" "db 230" "db 102" "db 88" "db 102" "db 72" "db 102" "db 131" "db 248" "db 0" "db 117" "db 188" "db 139" "db 53" "db 168" "db 10" "db 1" "db 0" "db 139" "db 254" "db 102" "db 185" "db 0" "db 3" "db 102" "db 186" "db 200" "db 3" "db 102" "db 184" "db 0" "db 0" "db 238" "db 252" "db 179" "db 54" "db 102" "db 184" "db 1" "db 18" "db 205" "db 16" "db 102" "db 186" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039f92_3(void);
#pragma aux __db_FUN_00039f92_3 = "db 218" "db 3" "db 236" "db 168" "db 8" "db 116" "db 251" "db 102" "db 186" "db 201" "db 3" "db 243" "db 110" "db 139" "db 254" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux flic_load_palette modify [eax ebx ecx edx esi edi ebp];
void flic_load_palette(void) {
    __db_FUN_00039f92_0();
    __db_FUN_00039f92_1();
    __db_FUN_00039f92_2();
    __db_FUN_00039f92_3();
}
