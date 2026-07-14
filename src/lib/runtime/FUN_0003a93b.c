/* C runtime: lseek @ 0x3a93b (CLIB3S). DOS seek (AH=0x42), 64-bit->32-bit result combine,
   error path calls the DOS-error handler. Full body via #pragma aux db bytes; the one call
   is a real `call FUN_0003c4b9` so its rel32 is a masked reloc. */
extern void FUN_0003c4b9(int);
extern long __lseek(void);
#pragma aux __lseek = "db 139" "db 69" "db 12" "db 139" "db 85" "db 16" "db 139" "db 77" "db 20" "db 102" "db 137" "db 195" "db 136" "db 200" "db 180" "db 66" "db 139" "db 202" "db 193" "db 233" "db 16" "db 205" "db 33" "db 102" "db 209" "db 210" "db 102" "db 209" "db 202" "db 193" "db 226" "db 16" "db 102" "db 139" "db 208" "db 137" "db 208" "db 133" "db 210" "db 125" "db 17" "db 15" "db 183" "db 194" "db 80" "call FUN_0003c4b9" "db 184" "db 255" "db 255" "db 255" "db 255" "db 131" "db 196" "db 4" parm [] value [eax] modify exact [eax ebx ecx edx];
long FUN_0003a93b(int handle, long offset, int origin)
{
    return __lseek();
}
