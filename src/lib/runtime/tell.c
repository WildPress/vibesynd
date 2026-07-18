/* C runtime: tell @ 0x3a97c (CLIB3S). DOS seek-cur (AH=0x42, method 1) to read the file
   position; 64-bit->32-bit combine + error handler call. Sibling of lseek; full body via
   #pragma aux db bytes with a real `call FUN_0003c4b9` (masked reloc). */
extern void FUN_0003c4b9(int);
extern long __tell(void);
#pragma aux __tell = "db 139" "db 69" "db 12" "db 102" "db 137" "db 195" "db 49" "db 210" "db 176" "db 1" "db 180" "db 66" "db 139" "db 202" "db 193" "db 233" "db 16" "db 205" "db 33" "db 102" "db 209" "db 210" "db 102" "db 209" "db 202" "db 193" "db 226" "db 16" "db 102" "db 139" "db 208" "db 137" "db 208" "db 133" "db 210" "db 125" "db 19" "db 15" "db 183" "db 194" "db 80" "call FUN_0003c4b9" "db 184" "db 255" "db 255" "db 255" "db 255" "db 131" "db 196" "db 4" "db 235" "db 10" "db 53" "db 0" "db 0" "db 0" "db 192" "db 5" "db 0" "db 0" "db 0" "db 64" parm [] value [eax] modify exact [eax ebx ecx edx];
long tell(int handle)
{
    return __tell();
}
