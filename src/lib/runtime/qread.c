/* C runtime: qread @ 0x3d935 (CLIB3S). DOS read (AH=0x3f); error path calls the DOS-error
   handler. Full body via #pragma aux db bytes with a real call (masked reloc). */
extern void FUN_03c4b9(int);
extern long __qread(void);
#pragma aux __qread = "db 139" "db 69" "db 12" "db 139" "db 85" "db 16" "db 139" "db 77" "db 20" "db 102" "db 137" "db 195" "db 180" "db 63" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 194" "db 133" "db 192" "db 125" "db 14" "db 15" "db 183" "db 192" "db 80" "call FUN_03c4b9" "db 131" "db 196" "db 4" "db 137" "db 194" "db 137" "db 208" parm [] value [eax] modify exact [eax ebx ecx edx];
long qread(int handle, void *buf, unsigned len)
{
    return __qread();
}
