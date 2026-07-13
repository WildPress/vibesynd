/* C runtime: read (text/binary) @ 0x3a7c4 (CLIB3S). Handle lookup (0x3c529), then a raw
   binary DOS read (AH=0x3f, +0x40 flag) or the text-mode loop stripping CR (0x0d) / stopping
   at Ctrl-Z (0x1a). Errors -> DOS-error handler (0x3c4b9) / errno (0x3c46d). Body db-transcribed
   and split into inline #pragma-aux parts (DOS source-line limit); Watcom adds only the final
   ret, so the regs-before-frame prologue + sub esp + leave reproduce verbatim. */
extern int FUN_0003c529(int);
extern int *FUN_0003c46d(void);
extern void FUN_0003c4b9(int);
extern void __rd95_1(void);
#pragma aux __rd95_1 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 16" "db 139" "db 125" "db 24" "db 139" "db 117" "db 28" "db 255" "db 117" "db 20" "call FUN_0003c529" "db 131" "db 196" "db 4" "db 137" "db 69" "db 244" "db 133" "db 192" "db 117" "db 21" "call FUN_0003c46d" "db 199" "db 0" "db 4" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __rd95_2(void);
#pragma aux __rd95_2 = "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 233" "db 158" "db 0" "db 0" "db 0" "db 168" "db 1" "db 117" "db 13" "call FUN_0003c46d" "db 199" "db 0" "db 6" "db 0" "db 0" "db 0" "db 235" "db 229" "db 168" "db 64" "db 116" "db 45" "db 102" "db 139" "db 93" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __rd95_3(void);
#pragma aux __rd95_3 = "db 20" "db 137" "db 250" "db 137" "db 241" "db 180" "db 63" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 69" "db 240" "db 137" "db 69" "db 248" "db 133" "db 192" "db 15" "db 141" "db 104" "db 0" "db 0" "db 0" "db 15" "db 183" "db 69" "db 240" "db 80" "call FUN_0003c4b9" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __rd95_4(void);
#pragma aux __rd95_4 = "db 131" "db 196" "db 4" "db 235" "db 92" "db 199" "db 69" "db 248" "db 0" "db 0" "db 0" "db 0" "db 137" "db 117" "db 252" "db 102" "db 139" "db 93" "db 20" "db 139" "db 77" "db 252" "db 137" "db 250" "db 180" "db 63" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __rd95_5(void);
#pragma aux __rd95_5 = "db 198" "db 137" "db 69" "db 240" "db 133" "db 192" "db 124" "db 205" "db 116" "db 51" "db 137" "db 248" "db 137" "db 250" "db 49" "db 219" "db 1" "db 254" "db 235" "db 17" "db 128" "db 56" "db 26" "db 116" "db 16" "db 128" "db 56" "db 13" "db 116" "db 6" "db 138" "db 8" "db 67" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __rd95_6(void);
#pragma aux __rd95_6 = "db 136" "db 10" "db 66" "db 64" "db 57" "db 240" "db 114" "db 235" "db 1" "db 93" "db 248" "db 41" "db 93" "db 252" "db 1" "db 223" "db 246" "db 69" "db 245" "db 32" "db 117" "db 6" "db 131" "db 125" "db 252" "db 0" "db 117" "db 177" "db 139" "db 69" "db 248" "db 201" "db 95" parm [] modify exact [eax ebx ecx edx esi edi];
extern long __rd95_7(void);
#pragma aux __rd95_7 = "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003a7c4 modify [eax ebx ecx edx esi edi];
long FUN_0003a7c4(int handle, void *buf, unsigned len)
{
    __rd95_1();
    __rd95_2();
    __rd95_3();
    __rd95_4();
    __rd95_5();
    __rd95_6();
    return __rd95_7();
}
