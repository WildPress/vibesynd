/* C runtime: getenv @ 0x3cba4 (CLIB3S, Watcom 9.5). Framed (push ebx/esi/edi/ebp;
   mov ebp,esp). Walks the environment table at g_c2f4; for the name arg at [ebp+0x14]
   it takes strlen (strlen) once, then for each entry compares that many bytes
   (strnicmp) and checks the following byte is '=' (0x3d); on a hit returns a pointer
   just past the '='. Returns NULL if environ is empty, the name is NULL, or no match.
   Whole body db-transcribed into a frameless wrapper; two CALL rel32s masked externs,
   g_c2f4 a literal db abs32; trailing RET supplied by the wrapper. */
extern void strlen(void);
extern void strnicmp(void);
extern void __FUN_0003cba4_0(void);
extern void __FUN_0003cba4_1(void);
#pragma aux __FUN_0003cba4_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 53" "db 244" "db 194" "db 0" "db 0" "db 133" "db 246" "db 116" "db 62" "db 131" "db 125" "db 20" "db 0" "db 116" "db 56" "db 255" "db 117" "db 20" "call strlen" "db 131" "db 196" "db 4" "db 137" "db 199" "db 235" "db 35" "db 87" "db 255" "db 117" "db 20" "db 83" "call strnicmp" "db 131" "db 196" "db 12" "db 133" "db 192" "db 117" "db 15" "db 141" "db 4" "db 59" "db 128" "db 56" "db 61" "db 117" "db 7" "db 141" "db 71" "db 1" "db 1" "db 216" "db 235" "db 11" "db 131" "db 198" "db 4" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cba4_1 = "db 139" "db 30" "db 133" "db 219" "db 117" "db 215" "db 49" "db 192" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux getenv modify [eax ebx ecx edx esi edi ebp];
void getenv(void)
{
    __FUN_0003cba4_0();
    __FUN_0003cba4_1();
}
