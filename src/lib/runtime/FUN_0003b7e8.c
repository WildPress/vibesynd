/* C runtime: _fopen core @ 0x3b7e8 (CLIB3S). Parse mode, open/create via 0x3a598, append-truncate via fseek, init FILE; regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void FUN_0003b739();
extern void tolower();
extern void FUN_0003a598();
extern void FUN_0003b594();
extern void FUN_0003dae1();
extern void chktty();
extern void __db_0003b7e8_0(void);
#pragma aux __db_0003b7e8_0 = "db 83" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 93" "db 24" "db 255" "db 117" "db 16" "db 128" "db 99" "db 12" "db 252" "call FUN_0003b739" "db 9" "db 67" "db 12" "db 139" "db 69" "db 16" "db 131" "db 196" "db 4" "db 15" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_1(void);
#pragma aux __db_0003b7e8_1 = "db 182" "db 0" "db 80" "call tolower" "db 131" "db 196" "db 4" "db 136" "db 69" "db 252" "db 60" "db 114" "db 117" "db 49" "db 49" "db 192" "db 246" "db 67" "db 12" "db 2" "db 116" "db 5" "db 184" "db 2" "db 0" "db 0" "db 0" "db 246" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_2(void);
#pragma aux __db_0003b7e8_2 = "db 67" "db 12" "db 64" "db 116" "db 5" "db 128" "db 204" "db 2" "db 235" "db 3" "db 128" "db 204" "db 1" "db 106" "db 0" "db 255" "db 117" "db 20" "db 80" "db 255" "db 117" "db 12" "call FUN_0003a598" "db 131" "db 196" "db 16" "db 137" "db 67" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_3(void);
#pragma aux __db_0003b7e8_3 = "db 16" "db 235" "db 86" "db 246" "db 67" "db 12" "db 1" "db 15" "db 149" "db 192" "db 15" "db 182" "db 192" "db 131" "db 192" "db 33" "db 128" "db 125" "db 252" "db 97" "db 117" "db 4" "db 12" "db 16" "db 235" "db 2" "db 12" "db 64" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_4(void);
#pragma aux __db_0003b7e8_4 = "db 246" "db 67" "db 12" "db 64" "db 116" "db 5" "db 128" "db 204" "db 2" "db 235" "db 3" "db 128" "db 204" "db 1" "db 104" "db 128" "db 1" "db 0" "db 0" "db 255" "db 117" "db 20" "db 80" "db 255" "db 117" "db 12" "call FUN_0003a598" "db 131" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_5(void);
#pragma aux __db_0003b7e8_5 = "db 196" "db 16" "db 137" "db 67" "db 16" "db 128" "db 125" "db 252" "db 97" "db 117" "db 18" "db 131" "db 248" "db 255" "db 116" "db 13" "db 106" "db 2" "db 106" "db 0" "db 83" "call FUN_0003b594" "db 131" "db 196" "db 12" "db 131" "db 123" "db 16" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b7e8_6(void);
#pragma aux __db_0003b7e8_6 = "db 255" "db 117" "db 13" "db 83" "call FUN_0003dae1" "db 131" "db 196" "db 4" "db 49" "db 192" "db 235" "db 22" "db 49" "db 192" "db 137" "db 67" "db 4" "db 137" "db 67" "db 8" "db 83" "db 137" "db 67" "db 20" "call chktty" "db 131" "db 196" "db 4" parm [] modify exact [eax ebx ecx edx esi edi];
extern void * __db_0003b7e8_7(void);
#pragma aux __db_0003b7e8_7 = "db 137" "db 216" "db 201" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003b7e8 modify [eax ebx ecx edx esi edi];
void * FUN_0003b7e8(char *name, char *mode, int share, void *fp)
{
    __db_0003b7e8_0();
    __db_0003b7e8_1();
    __db_0003b7e8_2();
    __db_0003b7e8_3();
    __db_0003b7e8_4();
    __db_0003b7e8_5();
    __db_0003b7e8_6();
    return __db_0003b7e8_7();
}
