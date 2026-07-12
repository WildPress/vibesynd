/* C runtime: isatty @ 0x3c44d (CLIB3S). DOS IOCTL 0x4400 get-device-info, test the
   char-device bit -> 0/1. int21 asm replicated via #pragma aux (db bytes). */
extern int __isatty(int handle);
#pragma aux __isatty = "db 102" "db 137" "db 195" "db 176" "db 0" "db 180" "db 68" "db 205" "db 33" "db 209" "db 210" "db 209" "db 202" "db 246" "db 194" "db 128" "db 15" "db 149" "db 192" "db 15" "db 182" "db 192" parm [eax] value [eax] modify exact [eax ebx ecx edx];
int FUN_0003c44d(int handle)
{
    return __isatty(handle);
}
