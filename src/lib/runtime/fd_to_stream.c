/* C runtime: fd->stream lookup @ 0x3c529 (CLIB3S, Watcom 9.5). Returns the stream table
   entry ds:0xc07c[fd*4] after (for fd 0..5, when the fd-flag 0x40 is clear) probing it
   with isatty (isatty): marks 0x40, and on a tty also marks 0x20. Out-of-range fd
   (>= ds:0xc028) returns 0. Hand-asm: full-body #pragma aux; one masked-reloc call; -d2
   frame (push ebx; push esi; push ebp; mov ebp,esp), fd at [ebp+0x10]. */
extern int isatty(int fd);
extern int __fdstream(void);
#pragma aux __fdstream = "db 139" "db 117" "db 16" "db 59" "db 53" "db 40" "db 192" "db 0" "db 0" "db 114" "db 4" "db 49" "db 192" "db 235" "db 58" "db 131" "db 254" "db 5" "db 127" "db 45" "db 137" "db 243" "db 161" "db 124" "db 192" "db 0" "db 0" "db 193" "db 227" "db 2" "db 246" "db 68" "db 3" "db 1" "db 64" "db 117" "db 28" "db 86" "db 128" "db 76" "db 3" "db 1" "db 64" "call isatty" "db 131" "db 196" "db 4" "db 133" "db 192" "db 116" "db 10" "db 161" "db 124" "db 192" "db 0" "db 0" "db 128" "db 76" "db 24" "db 1" "db 32" "db 161" "db 124" "db 192" "db 0" "db 0" "db 139" "db 4" "db 176" parm [] value [eax] modify exact [eax ebx esi];

int fd_to_stream(int fd)
{
    return __fdstream();
}
