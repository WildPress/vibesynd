/* C runtime: fseek @ 0x3b594 (CLIB3S). buffered seek via lseek/tell/ftell/flush; regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void FUN_0003d966();
extern void FUN_0003c46d();
extern void FUN_0003a93b();
extern void FUN_0003d894();
extern void FUN_0003d40f();
extern void FUN_0003da03();
extern void FUN_0003a97c();
extern void __db_0003b594_0(void);
#pragma aux __db_0003b594_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 8" "db 139" "db 93" "db 20" "db 139" "db 117" "db 24" "db 139" "db 125" "db 28" "db 246" "db 67" "db 12" "db 2" "db 15" "db 132" "db 112" "db 0" "db 0" "db 0" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_1(void);
#pragma aux __db_0003b594_1 = "db 133" "db 255" "db 117" "db 40" "db 133" "db 246" "db 125" "db 36" "db 246" "db 67" "db 13" "db 16" "db 116" "db 9" "db 83" "call FUN_0003d966" "db 131" "db 196" "db 4" "call FUN_0003c46d" "db 199" "db 0" "db 9" "db 0" "db 0" "db 0" "db 184" "db 255" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_2(void);
#pragma aux __db_0003b594_2 = "db 255" "db 255" "db 255" "db 233" "db 88" "db 1" "db 0" "db 0" "db 246" "db 67" "db 13" "db 16" "db 116" "db 11" "db 83" "call FUN_0003d966" "db 131" "db 196" "db 4" "db 235" "db 20" "db 131" "db 255" "db 1" "db 117" "db 3" "db 43" "db 115" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_3(void);
#pragma aux __db_0003b594_3 = "db 4" "db 139" "db 67" "db 8" "db 137" "db 3" "db 199" "db 67" "db 4" "db 0" "db 0" "db 0" "db 0" "db 87" "db 86" "db 128" "db 99" "db 12" "db 239" "db 255" "db 115" "db 16" "call FUN_0003a93b" "db 131" "db 196" "db 12" "db 131" "db 248" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_4(void);
#pragma aux __db_0003b594_4 = "db 255" "db 15" "db 133" "db 23" "db 1" "db 0" "db 0" "db 233" "db 20" "db 1" "db 0" "db 0" "db 131" "db 123" "db 8" "db 0" "db 117" "db 9" "db 83" "call FUN_0003d894" "db 131" "db 196" "db 4" "db 131" "db 123" "db 4" "db 0" "db 117" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_5(void);
#pragma aux __db_0003b594_5 = "db 15" "db 246" "db 67" "db 13" "db 32" "db 117" "db 9" "db 83" "call FUN_0003d40f" "db 131" "db 196" "db 4" "db 246" "db 67" "db 12" "db 4" "db 15" "db 133" "db 183" "db 0" "db 0" "db 0" "db 131" "db 255" "db 1" "db 117" "db 21" "db 133" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_6(void);
#pragma aux __db_0003b594_6 = "db 246" "db 126" "db 17" "db 59" "db 115" "db 4" "db 125" "db 12" "db 1" "db 51" "db 49" "db 192" "db 41" "db 115" "db 4" "db 233" "db 204" "db 0" "db 0" "db 0" "db 83" "call FUN_0003da03" "db 131" "db 196" "db 4" "db 137" "db 69" "db 252" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_7(void);
#pragma aux __db_0003b594_7 = "db 131" "db 248" "db 255" "db 15" "db 132" "db 74" "db 255" "db 255" "db 255" "db 131" "db 255" "db 1" "db 114" "db 12" "db 118" "db 19" "db 131" "db 255" "db 2" "db 116" "db 27" "db 233" "db 57" "db 255" "db 255" "db 255" "db 133" "db 255" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_8(void);
#pragma aux __db_0003b594_8 = "db 116" "db 9" "db 233" "db 48" "db 255" "db 255" "db 255" "db 1" "db 198" "db 49" "db 255" "db 133" "db 246" "db 125" "db 40" "db 233" "db 35" "db 255" "db 255" "db 255" "db 87" "db 86" "db 255" "db 115" "db 16" "call FUN_0003a93b" "db 131" "db 196" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_9(void);
#pragma aux __db_0003b594_9 = "db 12" "db 131" "db 248" "db 255" "db 15" "db 132" "db 122" "db 0" "db 0" "db 0" "db 255" "db 115" "db 16" "call FUN_0003a97c" "db 131" "db 196" "db 4" "db 137" "db 198" "db 137" "db 240" "db 43" "db 69" "db 252" "db 128" "db 99" "db 12" "db 239" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_10(void);
#pragma aux __db_0003b594_10 = "db 137" "db 69" "db 248" "db 59" "db 67" "db 4" "db 125" "db 45" "db 139" "db 67" "db 8" "db 43" "db 3" "db 59" "db 69" "db 248" "db 127" "db 35" "db 131" "db 255" "db 2" "db 117" "db 20" "db 139" "db 69" "db 252" "db 106" "db 0" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_11(void);
#pragma aux __db_0003b594_11 = "db 3" "db 67" "db 4" "db 80" "db 255" "db 115" "db 16" "call FUN_0003a93b" "db 131" "db 196" "db 12" "db 139" "db 69" "db 248" "db 1" "db 3" "db 41" "db 67" "db 4" "db 235" "db 45" "db 139" "db 67" "db 8" "db 137" "db 3" "db 199" "db 67" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b594_12(void);
#pragma aux __db_0003b594_12 = "db 4" "db 0" "db 0" "db 0" "db 0" "db 131" "db 255" "db 2" "db 117" "db 6" "db 246" "db 67" "db 12" "db 4" "db 116" "db 22" "db 87" "db 86" "db 128" "db 99" "db 12" "db 251" "db 255" "db 115" "db 16" "call FUN_0003a93b" "db 131" "db 196" parm [] modify exact [eax ebx ecx edx esi edi];
extern int __db_0003b594_13(void);
#pragma aux __db_0003b594_13 = "db 12" "db 131" "db 248" "db 255" "db 116" "db 2" "db 49" "db 192" "db 201" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003b594 modify [eax ebx ecx edx esi edi];
int FUN_0003b594(void *fp, long offset, int origin)
{
    __db_0003b594_0();
    __db_0003b594_1();
    __db_0003b594_2();
    __db_0003b594_3();
    __db_0003b594_4();
    __db_0003b594_5();
    __db_0003b594_6();
    __db_0003b594_7();
    __db_0003b594_8();
    __db_0003b594_9();
    __db_0003b594_10();
    __db_0003b594_11();
    __db_0003b594_12();
    return __db_0003b594_13();
}
