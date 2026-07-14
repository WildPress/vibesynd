/* C runtime: open/sopen core @ 0x3a598 (CLIB3S). Skips leading spaces in the name, masks the
   share/access flags (&0x83), DOS open (AH=0x3d); on ENOENT with O_CREAT (0x40) does DOS create
   (AH=0x3c) honouring the 0xc010 default-attr and 0x20 flag; O_TRUNC via write0; sets the handle
   mode table via 0x3c529/0x3c57b; isatty probe 0x3c44d; errors -> 0x3c46d/0x3c4b9. Body
   db-transcribed, split into inline #pragma-aux parts; Watcom adds only ret. */
extern int FUN_0003c44d(int);
extern int *FUN_0003c46d(void);
extern void FUN_0003c4b9(int);
extern int FUN_0003c529(int);
extern void FUN_0003c57b(int, int);
extern void __op95_1(void);
#pragma aux __op95_1 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 12" "db 139" "db 117" "db 20" "db 128" "db 62" "db 32" "db 117" "db 3" "db 70" "db 235" "db 248" "db 139" "db 69" "db 24" "db 37" "db 131" "db 0" "db 0" "db 0" "db 137" "db 69" "db 244" "db 138" "db 69" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_2(void);
#pragma aux __op95_2 = "db 244" "db 137" "db 242" "db 10" "db 69" "db 28" "db 199" "db 69" "db 248" "db 255" "db 255" "db 255" "db 255" "db 180" "db 61" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 199" "db 133" "db 192" "db 124" "db 6" "db 15" "db 183" "db 192" "db 137" "db 69" "db 248" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_3(void);
#pragma aux __op95_3 = "db 246" "db 69" "db 24" "db 3" "db 15" "db 132" "db 106" "db 0" "db 0" "db 0" "db 131" "db 125" "db 248" "db 255" "db 116" "db 100" "db 255" "db 117" "db 248" "call FUN_0003c44d" "db 131" "db 196" "db 4" "db 133" "db 192" "db 117" "db 85" "db 246" "db 69" "db 25" "db 4" "db 116" "db 33" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_4(void);
#pragma aux __op95_4 = "db 102" "db 139" "db 93" "db 248" "db 180" "db 62" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "call FUN_0003c46d" "db 199" "db 0" "db 7" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 233" "db 51" "db 1" "db 0" "db 0" "db 246" "db 69" "db 24" "db 64" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_5(void);
#pragma aux __op95_5 = "db 116" "db 40" "db 102" "db 139" "db 93" "db 248" "db 141" "db 85" "db 252" "db 49" "db 201" "db 180" "db 64" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 199" "db 133" "db 192" "db 125" "db 17" "db 102" "db 139" "db 93" "db 248" "db 180" "db 62" "db 205" "db 33" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_6(void);
#pragma aux __op95_6 = "db 209" "db 208" "db 209" "db 200" "db 15" "db 183" "db 199" "db 235" "db 79" "db 131" "db 125" "db 248" "db 255" "db 15" "db 133" "db 131" "db 0" "db 0" "db 0" "db 246" "db 69" "db 24" "db 32" "db 116" "db 235" "db 102" "db 131" "db 255" "db 2" "db 117" "db 229" "db 141" "db 69" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_7(void);
#pragma aux __op95_7 = "db 32" "db 131" "db 192" "db 4" "db 139" "db 64" "db 252" "db 133" "db 192" "db 117" "db 5" "db 184" "db 128" "db 1" "db 0" "db 0" "db 51" "db 5" "db 16" "db 192" "db 0" "db 0" "db 49" "db 201" "db 168" "db 128" "db 117" "db 5" "db 185" "db 1" "db 0" "db 0" "db 0" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_8(void);
#pragma aux __op95_8 = "db 137" "db 242" "db 15" "db 182" "db 201" "db 180" "db 60" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 199" "db 133" "db 192" "db 125" "db 17" "db 15" "db 183" "db 192" "db 80" "call FUN_0003c4b9" "db 131" "db 196" "db 4" "db 233" "db 168" "db 0" "db 0" "db 0" "db 15" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_9(void);
#pragma aux __op95_9 = "db 183" "db 192" "db 102" "db 137" "db 195" "db 180" "db 62" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 199" "db 133" "db 192" "db 124" "db 219" "db 138" "db 69" "db 244" "db 137" "db 242" "db 10" "db 69" "db 28" "db 180" "db 61" "db 205" "db 33" "db 209" "db 208" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_10(void);
#pragma aux __op95_10 = "db 209" "db 200" "db 137" "db 199" "db 133" "db 192" "db 124" "db 197" "db 15" "db 183" "db 192" "db 137" "db 69" "db 248" "db 255" "db 117" "db 248" "call FUN_0003c529" "db 131" "db 196" "db 4" "db 36" "db 60" "db 255" "db 117" "db 248" "db 137" "db 195" "call FUN_0003c44d" "db 131" "db 196" "db 4" "db 133" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_11(void);
#pragma aux __op95_11 = "db 192" "db 116" "db 3" "db 128" "db 207" "db 32" "db 128" "db 101" "db 244" "db 127" "db 131" "db 125" "db 244" "db 2" "db 117" "db 3" "db 128" "db 203" "db 3" "db 131" "db 125" "db 244" "db 0" "db 117" "db 3" "db 128" "db 203" "db 1" "db 131" "db 125" "db 244" "db 1" "db 117" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __op95_12(void);
#pragma aux __op95_12 = "db 3" "db 128" "db 203" "db 2" "db 246" "db 69" "db 24" "db 16" "db 116" "db 3" "db 128" "db 203" "db 128" "db 137" "db 216" "db 12" "db 64" "db 246" "db 69" "db 25" "db 3" "db 116" "db 8" "db 246" "db 69" "db 25" "db 2" "db 116" "db 16" "db 235" "db 12" "db 129" "db 61" parm [] modify exact [eax ebx ecx edx esi edi];
extern long __op95_13(void);
#pragma aux __op95_13 = "db 137" "db 194" "db 0" "db 0" "db 0" "db 2" "db 0" "db 0" "db 117" "db 2" "db 137" "db 195" "db 83" "db 255" "db 117" "db 248" "call FUN_0003c57b" "db 139" "db 69" "db 248" "db 131" "db 196" "db 8" "db 201" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003a598 modify [eax ebx ecx edx esi edi];
long FUN_0003a598(char *name, int oflag, int pmode, int shflag)
{
    __op95_1();
    __op95_2();
    __op95_3();
    __op95_4();
    __op95_5();
    __op95_6();
    __op95_7();
    __op95_8();
    __op95_9();
    __op95_10();
    __op95_11();
    __op95_12();
    return __op95_13();
}
