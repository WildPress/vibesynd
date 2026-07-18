/* C runtime: fread @ 0x3b420 (CLIB3S). size*count buffered read (_filbuf/qread core); regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void FUN_0003c46d();
extern void ioalloc();
extern void FUN_0003d90b();
extern void qread();
extern void FUN_0003d40f();
extern void fread();
extern void __db_0003b420_0(void);
#pragma aux __db_0003b420_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 125" "db 28" "db 139" "db 93" "db 32" "db 246" "db 67" "db 12" "db 1" "db 117" "db 22" "call FUN_0003c46d" "db 199" "db 0" "db 4" "db 0" "db 0" "db 0" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_1(void);
#pragma aux __db_0003b420_1 = "db 49" "db 192" "db 128" "db 75" "db 12" "db 32" "db 233" "db 233" "db 0" "db 0" "db 0" "db 15" "db 175" "db 125" "db 24" "db 133" "db 255" "db 117" "db 7" "db 137" "db 248" "db 233" "db 218" "db 0" "db 0" "db 0" "db 131" "db 123" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_2(void);
#pragma aux __db_0003b420_2 = "db 8" "db 0" "db 117" "db 9" "db 83" "call ioalloc" "db 131" "db 196" "db 4" "db 199" "db 69" "db 252" "db 0" "db 0" "db 0" "db 0" "db 246" "db 67" "db 12" "db 64" "db 15" "db 132" "db 148" "db 0" "db 0" "db 0" "db 137" "db 254" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_3(void);
#pragma aux __db_0003b420_3 = "db 131" "db 123" "db 4" "db 0" "db 116" "db 36" "db 139" "db 123" "db 4" "db 57" "db 247" "db 118" "db 2" "db 137" "db 247" "db 87" "db 255" "db 51" "db 1" "db 125" "db 252" "db 255" "db 117" "db 20" "db 41" "db 254" "call FUN_0003d90b" "db 131" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_4(void);
#pragma aux __db_0003b420_4 = "db 196" "db 12" "db 1" "db 59" "db 1" "db 125" "db 20" "db 41" "db 123" "db 4" "db 133" "db 246" "db 15" "db 132" "db 126" "db 0" "db 0" "db 0" "db 59" "db 115" "db 20" "db 115" "db 6" "db 246" "db 67" "db 13" "db 4" "db 116" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_5(void);
#pragma aux __db_0003b420_5 = "db 67" "db 137" "db 240" "db 246" "db 67" "db 13" "db 4" "db 117" "db 13" "db 129" "db 254" "db 0" "db 2" "db 0" "db 0" "db 118" "db 5" "db 128" "db 228" "db 254" "db 48" "db 192" "db 80" "db 255" "db 117" "db 20" "db 255" "db 115" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_6(void);
#pragma aux __db_0003b420_6 = "db 16" "call qread" "db 131" "db 196" "db 12" "db 131" "db 248" "db 255" "db 117" "db 6" "db 128" "db 75" "db 12" "db 32" "db 235" "db 68" "db 133" "db 192" "db 117" "db 6" "db 128" "db 75" "db 12" "db 16" "db 235" "db 58" "db 1" "db 69" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_7(void);
#pragma aux __db_0003b420_7 = "db 20" "db 1" "db 69" "db 252" "db 41" "db 198" "db 235" "db 128" "db 83" "call FUN_0003d40f" "db 131" "db 196" "db 4" "db 133" "db 192" "db 116" "db 35" "db 233" "db 110" "db 255" "db 255" "db 255" "db 139" "db 117" "db 20" "db 1" "db 247" "db 83" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b420_8(void);
#pragma aux __db_0003b420_8 = "call fread" "db 136" "db 6" "db 131" "db 196" "db 4" "db 246" "db 67" "db 12" "db 48" "db 117" "db 8" "db 70" "db 255" "db 69" "db 252" "db 57" "db 254" "db 117" "db 231" "db 139" "db 69" "db 252" "db 49" "db 210" "db 247" "db 117" "db 24" parm [] modify exact [eax ebx ecx edx esi edi];
extern unsigned __db_0003b420_9(void);
#pragma aux __db_0003b420_9 = "db 201" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003b420 modify [eax ebx ecx edx esi edi];
unsigned FUN_0003b420(void *buf, unsigned size, unsigned count, void *fp)
{
    __db_0003b420_0();
    __db_0003b420_1();
    __db_0003b420_2();
    __db_0003b420_3();
    __db_0003b420_4();
    __db_0003b420_5();
    __db_0003b420_6();
    __db_0003b420_7();
    __db_0003b420_8();
    return __db_0003b420_9();
}
