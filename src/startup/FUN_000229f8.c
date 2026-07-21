/* @ 0x229f8 (314B) -- db-transcription (uncarved callee; raw bytes, fixed-address link). */

extern void __db_FUN_000229f8_0(void);
#pragma aux __db_FUN_000229f8_0 = "db 83" "db 86" "db 139" "db 116" "db 36" "db 12" "db 102" "db 139" "db 92" "db 36" "db 16" "db 232" "db 0" "db 253" "db 255" "db 255" "db 102" "db 137" "db 29" "db 46" "db 11" "db 1" "db 0" "db 129" "db 227" "db 255" "db 255" "db 0" "db 0" "db 185" "db 10" "db 0" "db 0" "db 0" "db 137" "db 218" "db 137" "db 216" "db 193" "db 250" "db 31" "db 247" "db 249" "db 137" "db 218" "db 131" "db 192" "db 48" "db 193" "db 250" "db 31" "db 136" "db 70" "db 9" "db 137" "db 216" "db 247" "db 249" "db 131" "db 194" "db 48" "db 138" "db 37" "db 5" "db 1" "db 0" "db 0" "db 136" "db 86" "db 10" "db 128" "db 252" "db 2" "db 15" "db 133" "db 170" "db 0" "db 0" "db 0" "db 186" modify exact [eax ebx ecx edx esi edi];
extern void __db_FUN_000229f8_1(void);
#pragma aux __db_FUN_000229f8_1 = "db 128" "db 12" "db 10" "db 0" "db 139" "db 29" "db 104" "db 83" "db 0" "db 0" "db 104" "db 8" "db 1" "db 0" "db 0" "db 141" "db 131" "db 8" "db 244" "db 1" "db 0" "db 137" "db 21" "db 108" "db 83" "db 0" "db 0" "db 86" "db 129" "db 195" "db 24" "db 244" "db 1" "db 0" "db 163" "db 116" "db 83" "db 0" "db 0" "db 137" "db 29" "db 112" "db 83" "db 0" "db 0" "db 232" "db 110" "db 94" "db 255" "db 255" "db 102" "db 139" "db 21" "db 48" "db 190" "db 1" "db 0" "db 131" "db 196" "db 8" "db 102" "db 133" "db 210" "db 117" "db 9" "db 102" "db 199" "db 5" "db 48" "db 190" "db 1" "db 0" "db 1" "db 0" "db 49" "db 219" "db 102" "db 139" "db 29" "db 48" modify exact [eax ebx ecx edx esi edi];
extern void __db_FUN_000229f8_2(void);
#pragma aux __db_FUN_000229f8_2 = "db 190" "db 1" "db 0" "db 185" "db 10" "db 0" "db 0" "db 0" "db 137" "db 218" "db 137" "db 216" "db 193" "db 250" "db 31" "db 247" "db 249" "db 131" "db 192" "db 48" "db 137" "db 218" "db 162" "db 60" "db 63" "db 0" "db 0" "db 193" "db 250" "db 31" "db 137" "db 216" "db 247" "db 249" "db 131" "db 194" "db 48" "db 104" "db 8" "db 63" "db 0" "db 0" "db 136" "db 21" "db 61" "db 63" "db 0" "db 0" "db 232" "db 107" "db 88" "db 255" "db 255" "db 138" "db 21" "db 74" "db 11" "db 1" "db 0" "db 131" "db 196" "db 4" "db 132" "db 210" "db 116" "db 18" "db 104" "db 192" "db 64" "db 0" "db 0" "db 232" "db 84" "db 88" "db 255" "db 255" "db 131" "db 196" "db 4" "db 232" modify exact [eax ebx ecx edx esi edi];
extern void __db_FUN_000229f8_3(void);
#pragma aux __db_FUN_000229f8_3 = "db 124" "db 51" "db 1" "db 0" "db 232" "db 120" "db 165" "db 2" "db 0" "db 128" "db 61" "db 5" "db 1" "db 0" "db 0" "db 4" "db 117" "db 48" "db 104" "db 8" "db 1" "db 0" "db 0" "db 86" "db 232" "db 227" "db 93" "db 255" "db 255" "db 102" "db 139" "db 13" "db 48" "db 190" "db 1" "db 0" "db 131" "db 196" "db 8" "db 102" "db 133" "db 201" "db 117" "db 9" "db 102" "db 199" "db 5" "db 48" "db 190" "db 1" "db 0" "db 1" "db 0" "db 104" "db 68" "db 65" "db 0" "db 0" "db 232" "db 17" "db 88" "db 255" "db 255" "db 131" "db 196" "db 4" "db 232" "db 41" "db 253" "db 255" "db 255" "db 94" "db 91" "db 195" modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_000229f8 modify [eax ebx ecx edx esi edi] aborts;
void FUN_000229f8(void) {
    __db_FUN_000229f8_0();
    __db_FUN_000229f8_1();
    __db_FUN_000229f8_2();
    __db_FUN_000229f8_3();
}
