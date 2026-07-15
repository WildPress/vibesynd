/* @ 0x3afa4 (122B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003afa4_0(void);
#pragma aux __db_FUN_0003afa4_0 = "db 235" "db 120" "db 87" "db 65" "db 84" "db 67" "db 79" "db 77" "db 32" "db 67" "db 47" "db 67" "db 43" "db 43" "db 51" "db 50" "db 32" "db 82" "db 117" "db 110" "db 45" "db 84" "db 105" "db 109" "db 101" "db 32" "db 115" "db 121" "db 115" "db 116" "db 101" "db 109" "db 46" "db 32" "db 40" "db 99" "db 41" "db 32" "db 67" "db 111" "db 112" "db 121" "db 114" "db 105" "db 103" "db 104" "db 116" "db 32" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003afa4_1(void);
#pragma aux __db_FUN_0003afa4_1 = "db 98" "db 121" "db 32" "db 87" "db 65" "db 84" "db 67" "db 79" "db 77" "db 32" "db 73" "db 110" "db 116" "db 101" "db 114" "db 110" "db 97" "db 116" "db 105" "db 111" "db 110" "db 97" "db 108" "db 32" "db 67" "db 111" "db 114" "db 112" "db 46" "db 32" "db 49" "db 57" "db 56" "db 56" "db 45" "db 49" "db 57" "db 57" "db 51" "db 46" "db 32" "db 65" "db 108" "db 108" "db 32" "db 114" "db 105" "db 103" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003afa4_2(void);
#pragma aux __db_FUN_0003afa4_2 = "db 104" "db 116" "db 115" "db 32" "db 114" "db 101" "db 115" "db 101" "db 114" "db 118" "db 101" "db 100" "db 46" "db 144" "db 144" "db 144" "db 3" "db 0" "db 0" "db 0" "db 0" "db 0" "db 99" "db 111" "db 110" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003afa4 modify [eax ebx ecx edx esi edi ebp] aborts;
void FUN_0003afa4(void) {
    __db_FUN_0003afa4_0();
    __db_FUN_0003afa4_1();
    __db_FUN_0003afa4_2();
}
