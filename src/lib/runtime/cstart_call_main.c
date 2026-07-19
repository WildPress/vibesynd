/* @ 0x3cfe6 (80B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003cfe6_0(void);
#pragma aux __db_FUN_0003cfe6_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 29" "db 196" "db 194" "db 0" "db 0" "db 131" "db 195" "db 3" "db 128" "db 227" "db 252" "db 232" "db 192" "db 250" "db 255" "db 255" "db 57" "db 195" "db 115" "db 8" "db 137" "db 216" "db 41" "db 196" "db 137" "db 224" "db 235" "db 2" "db 49" "db 192" "db 139" "db 29" "db 196" "db 194" "db 0" "db 0" "db 1" "db 216" "db 163" "db 200" "db 194" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003cfe6_1(void);
#pragma aux __db_FUN_0003cfe6_1 = "db 232" "db 143" "db 20" "db 0" "db 0" "db 255" "db 53" "db 84" "db 30" "db 1" "db 0" "db 255" "db 53" "db 88" "db 30" "db 1" "db 0" "db 232" "db 188" "db 123" "db 254" "db 255" "db 131" "db 196" "db 8" "db 80" "db 233" "db 84" "db 221" "db 255" "db 255" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux cstart_call_main modify [eax ebx ecx edx esi edi ebp] aborts;
void cstart_call_main(void) {
    __db_FUN_0003cfe6_0();
    __db_FUN_0003cfe6_1();
}
