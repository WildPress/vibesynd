/* C runtime: write (text/binary) @ 0x3ab8e (CLIB3S). Handle lookup (0x3c529); append-mode
   pre-seek (AH=0x42) for +0x80 handles; binary path is a single DOS write (AH=0x40), text path
   buffers into a stack blob translating LF (0x0a) -> CR/LF (0x0d) and flushes in chunks. Stack
   probe via 0x3cabb/0x3cafb; errors -> errno (0x3c46d)/DOS-error handler (0x3c4b9). Body
   db-transcribed, split into inline #pragma-aux parts; Watcom adds only ret (regs-before-frame
   prologue + sub esp + leave reproduced verbatim). */
extern int FUN_0003c529(int);
extern int *FUN_0003c46d(void);
extern void FUN_0003c4b9(int);
extern unsigned FUN_0003cabb(void);
extern void FUN_0003cafb(void);
extern void __wr95_1(void);
#pragma aux __wr95_1 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 24" "db 255" "db 117" "db 20" "call FUN_0003c529" "db 131" "db 196" "db 4" "db 137" "db 69" "db 236" "db 133" "db 192" "db 117" "db 21" "call FUN_0003c46d" "db 199" "db 0" "db 4" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_2(void);
#pragma aux __wr95_2 = "db 255" "db 255" "db 233" "db 163" "db 1" "db 0" "db 0" "db 168" "db 2" "db 117" "db 13" "call FUN_0003c46d" "db 199" "db 0" "db 6" "db 0" "db 0" "db 0" "db 235" "db 229" "db 168" "db 128" "db 116" "db 54" "db 102" "db 139" "db 93" "db 20" "db 176" "db 2" "db 49" "db 210" "db 180" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_3(void);
#pragma aux __wr95_3 = "db 66" "db 139" "db 202" "db 193" "db 233" "db 16" "db 205" "db 33" "db 102" "db 209" "db 210" "db 102" "db 209" "db 202" "db 193" "db 226" "db 16" "db 102" "db 139" "db 208" "db 137" "db 85" "db 232" "db 133" "db 210" "db 125" "db 18" "db 15" "db 183" "db 69" "db 232" "db 80" "call FUN_0003c4b9" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_4(void);
#pragma aux __wr95_4 = "db 131" "db 196" "db 4" "db 233" "db 88" "db 1" "db 0" "db 0" "db 246" "db 69" "db 236" "db 64" "db 116" "db 55" "db 102" "db 139" "db 93" "db 20" "db 139" "db 85" "db 24" "db 139" "db 77" "db 28" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_5(void);
#pragma aux __wr95_5 = "db 198" "db 137" "db 69" "db 232" "db 133" "db 192" "db 124" "db 205" "db 59" "db 69" "db 28" "db 15" "db 132" "db 43" "db 1" "db 0" "db 0" "call FUN_0003c46d" "db 137" "db 117" "db 28" "db 199" "db 0" "db 12" "db 0" "db 0" "db 0" "db 233" "db 24" "db 1" "db 0" "db 0" "call FUN_0003cabb" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_6(void);
#pragma aux __wr95_6 = "db 137" "db 195" "db 61" "db 176" "db 0" "db 0" "db 0" "db 115" "db 5" "call FUN_0003cafb" "db 191" "db 0" "db 2" "db 0" "db 0" "db 129" "db 251" "db 48" "db 2" "db 0" "db 0" "db 115" "db 5" "db 191" "db 128" "db 0" "db 0" "db 0" "db 137" "db 248" "db 41" "db 196" "db 137" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_7(void);
#pragma aux __wr95_7 = "db 224" "db 137" "db 69" "db 240" "db 49" "db 192" "db 137" "db 69" "db 252" "db 137" "db 69" "db 248" "db 139" "db 69" "db 24" "db 49" "db 246" "db 137" "db 69" "db 244" "db 139" "db 69" "db 252" "db 59" "db 69" "db 28" "db 15" "db 131" "db 150" "db 0" "db 0" "db 0" "db 139" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_8(void);
#pragma aux __wr95_8 = "db 69" "db 244" "db 128" "db 56" "db 10" "db 117" "db 74" "db 139" "db 69" "db 240" "db 198" "db 4" "db 6" "db 13" "db 70" "db 57" "db 254" "db 117" "db 62" "db 102" "db 139" "db 93" "db 20" "db 137" "db 194" "db 137" "db 249" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_9(void);
#pragma aux __wr95_9 = "db 209" "db 200" "db 137" "db 198" "db 137" "db 69" "db 232" "db 133" "db 192" "db 15" "db 140" "db 54" "db 255" "db 255" "db 255" "db 57" "db 248" "db 116" "db 21" "call FUN_0003c46d" "db 199" "db 0" "db 12" "db 0" "db 0" "db 0" "db 139" "db 69" "db 248" "db 1" "db 240" "db 233" "db 135" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_10(void);
#pragma aux __wr95_10 = "db 0" "db 0" "db 0" "db 139" "db 69" "db 252" "db 49" "db 246" "db 137" "db 69" "db 248" "db 139" "db 93" "db 244" "db 139" "db 69" "db 240" "db 138" "db 19" "db 255" "db 69" "db 252" "db 136" "db 20" "db 6" "db 70" "db 255" "db 69" "db 244" "db 57" "db 254" "db 117" "db 140" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_11(void);
#pragma aux __wr95_11 = "db 102" "db 139" "db 93" "db 20" "db 137" "db 194" "db 137" "db 249" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 198" "db 137" "db 69" "db 232" "db 133" "db 192" "db 15" "db 140" "db 226" "db 254" "db 255" "db 255" "db 57" "db 248" "db 117" "db 172" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_12(void);
#pragma aux __wr95_12 = "db 139" "db 69" "db 252" "db 49" "db 246" "db 137" "db 69" "db 248" "db 233" "db 94" "db 255" "db 255" "db 255" "db 133" "db 246" "db 116" "db 52" "db 102" "db 139" "db 93" "db 20" "db 139" "db 85" "db 240" "db 137" "db 241" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" "db 209" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __wr95_13(void);
#pragma aux __wr95_13 = "db 200" "db 137" "db 199" "db 137" "db 69" "db 232" "db 133" "db 192" "db 15" "db 140" "db 175" "db 254" "db 255" "db 255" "db 57" "db 240" "db 116" "db 18" "call FUN_0003c46d" "db 199" "db 0" "db 12" "db 0" "db 0" "db 0" "db 139" "db 69" "db 248" "db 1" "db 248" "db 235" "db 3" "db 139" parm [] modify exact [eax ebx ecx edx esi edi];
extern long __wr95_14(void);
#pragma aux __wr95_14 = "db 69" "db 28" "db 201" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003ab8e modify [eax ebx ecx edx esi edi];
long FUN_0003ab8e(int handle, void *buf, unsigned len)
{
    __wr95_1();
    __wr95_2();
    __wr95_3();
    __wr95_4();
    __wr95_5();
    __wr95_6();
    __wr95_7();
    __wr95_8();
    __wr95_9();
    __wr95_10();
    __wr95_11();
    __wr95_12();
    __wr95_13();
    return __wr95_14();
}
