/* C runtime: printf conversion-specifier dispatcher @ 0x3c019 (CLIB3S, Watcom 9.5).
   The %-format engine core. Args at [ebp+0x18..0x20]; a 0x14-byte local frame and a
   pushed ES make its own prologue (push ebx;esi;edi;ES;ebp; mov ebp,esp; sub esp,0x14).
   Reads the conversion letter [ebx+0x15], routes through two 0x45..0x78 comparison
   ladders to per-specifier handlers (c/d/i/o/u/x/X/e/f/g/E/G/s/S/p/n), fetches the
   vararg (byte/word/dword/far ptr per the 0x10/0x20/0x40/0x80 length flags at [ebx+0x14]),
   calls the numeric formatters (0x3dcb5 ltoa-ish, 0x3dbeb, 0x3dc68, 0x3bee3 hex, 0x3be93,
   0x3bebc, 0x3bf36, 0x3c002, 0x3c42d) and finally TAIL-JUMPS to 0x3bd31 (never returns).
   Frameless full-frame db-transcription: own prologue/locals in the body, register
   inputs read from the stack via ebp; external calls are masked-reloc `call FUN_xxxx`;
   the closing `jmp FUN_0003bd31` + `aborts` on the last helper suppresses a wrapper RET
   so the byte count stays exact. Split across several #pragma aux (DOS line limit;
   Watcom concatenates them contiguously so every internal jump resolves). */
extern void FUN_0003bd31(void);
extern void FUN_0003be93(void);
extern void FUN_0003bebc(void);
extern void FUN_0003bee3(void);
extern void FUN_0003bf36(void);
extern void FUN_0003c002(void);
extern void FUN_0003c42d(void);
extern void FUN_0003dbeb(void);
extern void FUN_0003dc68(void);
extern void FUN_0003dcb5(void);
extern void __c019_0(void);
extern void __c019_1(void);
extern void __c019_2(void);
extern void __c019_3(void);
extern void __c019_4(void);
extern void __c019_5(void);
extern void __c019_6(void);
extern void __c019_7(void);
extern void __c019_8(void);
extern void __c019_9(void);
extern void __c019_10(void);
extern void __c019_11(void);
#pragma aux __c019_0 = "db 83" "db 86" "db 87" "db 6" "db 85" "db 137" "db 229" "db 131" "db 236" "db 20" "db 139" "db 117" "db 24" "db 139" "db 69" "db 28" "db 139" "db 93" "db 32" "db 199" "db 69" "db 248" "db 1" "db 0" "db 0" "db 0" "db 199" "db 67" "db 12" "db 0" "db 0" "db 0" "db 0" "db 140" "db 93" "db 252" "db 138" "db 83" "db 21" "db 137" "db 247" "db 128" "db 250" "db 105" "db 114" "db 25" "db 15" "db 134" "db 81" "db 0" "db 0" "db 0" "db 128" "db 250" "db 117" "db 114" "db 9" "db 118" "db 26" "db 128" "db 250" "db 120" "db 116" "db 21" "db 235" "db 77" "db 128" "db 250" "db 111" "db 235" "db 247" "db 128" "db 250" "db 88" "db 114" "db 67" "db 118" "db 7" "db 128" "db 250" "db 100" "db 116" "db 50" "db 235" "db 58" "db 246" "db 67" "db 20" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_1 = "db 32" "db 116" "db 10" "db 131" "db 0" "db 4" "db 139" "db 16" "db 139" "db 82" "db 252" "db 235" "db 25" "db 246" "db 67" "db 20" "db 16" "db 116" "db 11" "db 131" "db 0" "db 4" "db 139" "db 16" "db 15" "db 183" "db 82" "db 252" "db 235" "db 8" "db 131" "db 0" "db 4" "db 139" "db 16" "db 139" "db 82" "db 252" "db 137" "db 85" "db 240" "db 128" "db 99" "db 20" "db 249" "db 131" "db 123" "db 8" "db 255" "db 116" "db 4" "db 198" "db 67" "db 22" "db 32" "db 138" "db 83" "db 21" "db 199" "db 69" "db 244" "db 10" "db 0" "db 0" "db 0" "db 128" "db 250" "db 101" "db 114" "db 96" "db 15" "db 134" "db 208" "db 0" "db 0" "db 0" "db 128" "db 250" "db 111" "db 114" "db 60" "db 15" "db 134" "db 223" "db 0" "db 0" "db 0" "db 128" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_2 = "db 250" "db 115" "db 114" "db 35" "db 15" "db 134" "db 89" "db 1" "db 0" "db 0" "db 128" "db 250" "db 117" "db 15" "db 130" "db 164" "db 2" "db 0" "db 0" "db 15" "db 134" "db 230" "db 1" "db 0" "db 0" "db 128" "db 250" "db 120" "db 15" "db 132" "db 188" "db 1" "db 0" "db 0" "db 233" "db 144" "db 2" "db 0" "db 0" "db 128" "db 250" "db 112" "db 15" "db 132" "db 246" "db 1" "db 0" "db 0" "db 233" "db 130" "db 2" "db 0" "db 0" "db 128" "db 250" "db 103" "db 114" "db 98" "db 15" "db 134" "db 132" "db 0" "db 0" "db 0" "db 128" "db 250" "db 105" "db 15" "db 132" "db 216" "db 0" "db 0" "db 0" "db 233" "db 105" "db 2" "db 0" "db 0" "db 128" "db 250" "db 80" "db 114" "db 51" "db 15" "db 134" "db 205" "db 1" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_3 = "db 0" "db 128" "db 250" "db 88" "db 114" "db 26" "db 15" "db 134" "db 122" "db 1" "db 0" "db 0" "db 128" "db 250" "db 99" "db 15" "db 130" "db 74" "db 2" "db 0" "db 0" "db 15" "db 134" "db 48" "db 2" "db 0" "db 0" "db 233" "db 169" "db 0" "db 0" "db 0" "db 128" "db 250" "db 83" "db 15" "db 132" "db 226" "db 0" "db 0" "db 0" "db 233" "db 49" "db 2" "db 0" "db 0" "db 128" "db 250" "db 70" "db 114" "db 12" "db 118" "db 15" "db 128" "db 250" "db 71" "db 116" "db 50" "db 233" "db 32" "db 2" "db 0" "db 0" "db 128" "db 250" "db 69" "db 235" "db 244" "db 246" "db 67" "db 20" "db 16" "db 116" "db 34" "db 131" "db 0" "db 4" "db 139" "db 0" "db 83" "db 139" "db 64" "db 252" "db 80" "db 86" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_4 = "call FUN_0003bf36" "db 131" "db 196" "db 12" "db 140" "db 93" "db 252" "db 106" "db 255" "db 15" "db 183" "db 69" "db 252" "db 80" "db 86" "db 137" "db 247" "db 235" "db 80" "db 83" "db 80" "db 86" "call FUN_0003c002" "db 131" "db 196" "db 12" "db 102" "db 137" "db 85" "db 252" "db 106" "db 255" "db 15" "db 183" "db 210" "db 82" "db 80" "db 137" "db 199" "db 235" "db 54" "db 184" "db 8" "db 0" "db 0" "db 0" "db 246" "db 67" "db 20" "db 1" "db 116" "db 4" "db 198" "db 6" "db 48" "db 70" "db 80" "db 86" "db 255" "db 117" "db 240" "call FUN_0003dcb5" "db 131" "db 196" "db 12" "db 131" "db 123" "db 8" "db 0" "db 117" "db 12" "db 128" "db 62" "db 48" "db 117" "db 7" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_5 = "db 142" "db 69" "db 252" "db 38" "db 198" "db 7" "db 0" "db 106" "db 255" "db 15" "db 183" "db 69" "db 252" "db 80" "db 87" "call FUN_0003be93" "db 131" "db 196" "db 12" "db 233" "db 182" "db 1" "db 0" "db 0" "db 246" "db 67" "db 20" "db 32" "db 116" "db 20" "db 131" "db 0" "db 4" "db 255" "db 117" "db 244" "db 139" "db 0" "db 86" "db 139" "db 64" "db 252" "db 80" "call FUN_0003dcb5" "db 235" "db 188" "db 246" "db 67" "db 20" "db 16" "db 116" "db 15" "db 131" "db 0" "db 4" "db 255" "db 117" "db 244" "db 139" "db 0" "db 86" "db 15" "db 191" "db 64" "db 252" "db 235" "db 227" "db 255" "db 117" "db 244" "db 131" "db 0" "db 4" "db 86" "db 139" "db 0" "db 255" "db 112" "db 252" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_6 = "call FUN_0003dbeb" "db 235" "db 148" "db 246" "db 67" "db 20" "db 128" "db 116" "db 18" "db 131" "db 0" "db 8" "db 139" "db 0" "db 102" "db 139" "db 80" "db 252" "db 139" "db 120" "db 248" "db 102" "db 137" "db 85" "db 252" "db 235" "db 15" "db 246" "db 67" "db 20" "db 64" "db 131" "db 0" "db 4" "db 139" "db 0" "db 140" "db 93" "db 252" "db 139" "db 120" "db 252" "db 128" "db 99" "db 20" "db 249" "db 199" "db 69" "db 248" "db 0" "db 0" "db 0" "db 0" "db 128" "db 123" "db 21" "db 83" "db 117" "db 34" "db 246" "db 67" "db 20" "db 32" "db 116" "db 15" "db 142" "db 69" "db 252" "db 38" "db 15" "db 183" "db 7" "db 131" "db 199" "db 2" "db 233" "db 39" "db 1" "db 0" "db 0" "db 142" "db 69" "db 252" "db 38" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_7 = "db 15" "db 182" "db 7" "db 71" "db 233" "db 26" "db 1" "db 0" "db 0" "db 246" "db 67" "db 20" "db 32" "db 116" "db 19" "db 255" "db 115" "db 8" "db 15" "db 183" "db 69" "db 252" "db 80" "db 87" "call FUN_0003bebc" "db 233" "db 67" "db 255" "db 255" "db 255" "db 255" "db 115" "db 8" "db 233" "db 48" "db 255" "db 255" "db 255" "db 246" "db 67" "db 20" "db 1" "db 116" "db 20" "db 131" "db 125" "db 240" "db 0" "db 116" "db 14" "db 198" "db 67" "db 23" "db 48" "db 138" "db 67" "db 21" "db 136" "db 67" "db 24" "db 198" "db 67" "db 25" "db 0" "db 199" "db 69" "db 244" "db 16" "db 0" "db 0" "db 0" "db 255" "db 117" "db 244" "db 86" "db 255" "db 117" "db 240" "call FUN_0003dc68" "db 131" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_8 = "db 196" "db 12" "db 128" "db 123" "db 21" "db 88" "db 15" "db 133" "db 226" "db 254" "db 255" "db 255" "db 86" "call FUN_0003c42d" "db 131" "db 196" "db 4" "db 233" "db 212" "db 254" "db 255" "db 255" "db 131" "db 123" "db 4" "db 0" "db 117" "db 22" "db 246" "db 67" "db 20" "db 128" "db 116" "db 9" "db 199" "db 67" "db 4" "db 13" "db 0" "db 0" "db 0" "db 235" "db 7" "db 199" "db 67" "db 4" "db 8" "db 0" "db 0" "db 0" "db 128" "db 99" "db 20" "db 249" "db 131" "db 0" "db 4" "db 139" "db 16" "db 139" "db 82" "db 252" "db 137" "db 85" "db 236" "db 246" "db 67" "db 20" "db 128" "db 116" "db 40" "db 131" "db 0" "db 4" "db 139" "db 0" "db 106" "db 4" "db 139" "db 64" "db 252" "db 86" "db 37" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_9 = "db 255" "db 0" "db 0" "db 80" "call FUN_0003bee3" "db 131" "db 196" "db 12" "db 106" "db 8" "db 141" "db 70" "db 5" "db 80" "db 255" "db 117" "db 236" "db 198" "db 70" "db 4" "db 58" "db 235" "db 4" "db 106" "db 8" "db 86" "db 82" "call FUN_0003bee3" "db 131" "db 196" "db 12" "db 128" "db 123" "db 21" "db 80" "db 15" "db 133" "db 119" "db 254" "db 255" "db 255" "db 86" "call FUN_0003c42d" "db 131" "db 196" "db 4" "db 233" "db 105" "db 254" "db 255" "db 255" "db 131" "db 0" "db 4" "db 139" "db 0" "db 138" "db 64" "db 252" "db 136" "db 6" "db 49" "db 192" "db 136" "db 70" "db 1" "db 137" "db 69" "db 248" "db 235" "db 16" "db 49" "db 210" "db 137" "db 83" "db 4" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_10 = "db 138" "db 67" "db 21" "db 136" "db 6" "db 136" "db 86" "db 1" "db 137" "db 85" "db 248" "db 199" "db 67" "db 8" "db 1" "db 0" "db 0" "db 0" "db 184" "db 1" "db 0" "db 0" "db 0" "db 128" "db 99" "db 20" "db 249" "db 131" "db 125" "db 248" "db 0" "db 116" "db 61" "db 142" "db 69" "db 252" "db 38" "db 128" "db 63" "db 45" "db 117" "db 8" "db 72" "db 198" "db 67" "db 23" "db 45" "db 71" "db 235" "db 22" "db 246" "db 67" "db 20" "db 2" "db 116" "db 6" "db 198" "db 67" "db 23" "db 32" "db 235" "db 10" "db 246" "db 67" "db 20" "db 4" "db 116" "db 8" "db 198" "db 67" "db 23" "db 43" "db 198" "db 67" "db 24" "db 0" "db 59" "db 67" "db 8" "db 126" "db 5" "db 137" "db 67" "db 8" "db 235" "db 8" "db 139" "db 115" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c019_11 = "db 8" "db 41" "db 198" "db 137" "db 115" "db 12" "db 128" "db 123" "db 22" "db 42" "db 117" "db 8" "db 198" "db 67" "db 23" "db 0" "db 128" "db 99" "db 20" "db 249" "db 131" "db 123" "db 8" "db 255" "db 116" "db 5" "db 59" "db 67" "db 8" "db 125" "db 9" "db 128" "db 123" "db 21" "db 99" "db 116" "db 3" "db 137" "db 67" "db 8" "db 140" "db 216" "db 106" "db 255" "db 15" "db 183" "db 192" "db 80" "db 141" "db 83" "db 23" "db 82" "call FUN_0003be93" "db 3" "db 67" "db 8" "db 102" "db 139" "db 85" "db 252" "db 3" "db 67" "db 12" "db 131" "db 196" "db 12" "db 41" "db 67" "db 4" "db 137" "db 248" "jmp FUN_0003bd31" modify exact [eax ebx ecx edx esi edi ebp] aborts;
#pragma aux FUN_0003c019 modify exact [eax ebx ecx edx esi edi ebp] aborts;
void FUN_0003c019(void)
{
    __c019_0();
    __c019_1();
    __c019_2();
    __c019_3();
    __c019_4();
    __c019_5();
    __c019_6();
    __c019_7();
    __c019_8();
    __c019_9();
    __c019_10();
    __c019_11();
}
