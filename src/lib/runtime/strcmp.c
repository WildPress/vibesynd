/* C runtime: strcmp @ 0x3a9c8 (CLIB3S). 386 hand-asm dword-at-a-time compare with the
   0xfefefeff/0x80808080 zero-byte detector, 4x unrolled, byte-tail resolve returning -1/0/1.
   Frameless leaf (saves edx/ebx/ecx). Body db-transcribed, split into inline #pragma-aux parts;
   Watcom adds only the final ret. */
extern void __cmp95_1(void);
#pragma aux __cmp95_1 = "db 82" "db 139" "db 68" "db 36" "db 8" "db 139" "db 84" "db 36" "db 12" "db 83" "db 81" "db 139" "db 216" "db 59" "db 194" "db 116" "db 108" "db 139" "db 3" "db 139" "db 10" "db 59" "db 200" "db 117" "db 106" "db 247" "db 209" "db 5" "db 255" "db 254" "db 254" "db 254" "db 35" parm [] modify exact [eax ebx ecx edx];
extern void __cmp95_2(void);
#pragma aux __cmp95_2 = "db 193" "db 37" "db 128" "db 128" "db 128" "db 128" "db 117" "db 84" "db 139" "db 67" "db 4" "db 139" "db 74" "db 4" "db 59" "db 200" "db 117" "db 80" "db 247" "db 209" "db 5" "db 255" "db 254" "db 254" "db 254" "db 35" "db 193" "db 37" "db 128" "db 128" "db 128" "db 128" "db 117" parm [] modify exact [eax ebx ecx edx];
extern void __cmp95_3(void);
#pragma aux __cmp95_3 = "db 58" "db 139" "db 67" "db 8" "db 139" "db 74" "db 8" "db 59" "db 200" "db 117" "db 54" "db 247" "db 209" "db 5" "db 255" "db 254" "db 254" "db 254" "db 35" "db 193" "db 37" "db 128" "db 128" "db 128" "db 128" "db 117" "db 32" "db 139" "db 67" "db 12" "db 139" "db 74" "db 12" parm [] modify exact [eax ebx ecx edx];
extern void __cmp95_4(void);
#pragma aux __cmp95_4 = "db 59" "db 200" "db 117" "db 28" "db 131" "db 195" "db 16" "db 131" "db 194" "db 16" "db 247" "db 209" "db 5" "db 255" "db 254" "db 254" "db 254" "db 35" "db 193" "db 37" "db 128" "db 128" "db 128" "db 128" "db 116" "db 148" "db 43" "db 192" "db 89" "db 91" "db 90" "db 195" "db 58" parm [] modify exact [eax ebx ecx edx];
extern void __cmp95_5(void);
#pragma aux __cmp95_5 = "db 193" "db 117" "db 29" "db 60" "db 0" "db 116" "db 242" "db 58" "db 229" "db 117" "db 21" "db 128" "db 252" "db 0" "db 116" "db 233" "db 193" "db 232" "db 16" "db 193" "db 233" "db 16" "db 58" "db 193" "db 117" "db 6" "db 60" "db 0" "db 116" "db 219" "db 58" "db 229" "db 27" parm [] modify exact [eax ebx ecx edx];
extern long __cmp95_6(void);
#pragma aux __cmp95_6 = "db 192" "db 12" "db 1" "db 89" "db 91" "db 90" parm [] value [eax] modify exact [eax ebx ecx edx];
#pragma aux strcmp modify [eax ebx ecx edx];
int strcmp(const char *s1, const char *s2)
{
    __cmp95_1();
    __cmp95_2();
    __cmp95_3();
    __cmp95_4();
    __cmp95_5();
    return __cmp95_6();
}
