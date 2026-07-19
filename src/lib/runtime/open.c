/* C runtime: open @ 0x3a579 (CLIB3S). Variadic wrapper: va_arg the mode, then
   call the real open (path, oflag, 0, mode). Full body via #pragma aux db bytes with a
   real `call sopen` (masked reloc). */
extern int sopen(int path, int oflag, int z, int mode);
extern int __open(void);
#pragma aux __open = "db 141" "db 69" "db 16" "db 131" "db 192" "db 4" "db 139" "db 64" "db 252" "db 80" "db 106" "db 0" "db 255" "db 117" "db 12" "db 255" "db 117" "db 8" "call sopen" "db 131" "db 196" "db 16" parm [] value [eax] modify exact [eax];
int open(int path, int oflag, int mode)
{
    return __open();
}
