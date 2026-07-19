/* C runtime: filelength @ 0x3ab1a (CLIB3S). Saves current pos (lseek SEEK_CUR),
   seeks to end (SEEK_END) for the length, restores original pos (SEEK_SET), returns
   the end position. Three cdecl calls to lseek (0x3a93b). Full body via #pragma aux
   db bytes with real `call lseek` (masked reloc); regs-before-frame prologue
   comes from the modify list, beating the 0x3a000+ prologue-order wall. */
extern long lseek(int, long, int);
extern long __flen(void);
#pragma aux __flen = "db 139" "db 125" "db 20" "db 106" "db 1" "db 106" "db 0" "db 87" "call lseek" "db 137" "db 195" "db 131" "db 196" "db 12" "db 131" "db 248" "db 255" "db 116" "db 29" "db 106" "db 2" "db 106" "db 0" "db 87" "call lseek" "db 131" "db 196" "db 12" "db 106" "db 0" "db 83" "db 87" "db 137" "db 198" "call lseek" "db 131" "db 196" "db 12" "db 137" "db 240" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
long filelength(int handle)
{
    return __flen();
}
