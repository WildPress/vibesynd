/* C runtime: DOS-error handler @ 0x3c4b9 (CLIB3S, Watcom 9.5). Called by lseek/tell/
   open/qread. Stores the raw DOS error (BL) via doserrno_ptr, then remaps the code
   (when < 0x100 and DOS major-version >= 3) through a small fixup ladder + the
   0xc014 errno table, else takes the high byte; writes the mapped code via errno_ptr
   and returns -1. Hand-asm: full-body #pragma aux with two masked-reloc calls; -d2
   supplies the frame (push ebx; push esi; push ebp; mov ebp,esp) and the arg at
   [ebp+0x10]. */
extern int *doserrno_ptr(void);
extern int *errno_ptr(void);
extern int  __doserr(void);
#pragma aux __doserr = "db 138" "db 93" "db 16" "db 15" "db 182" "db 243" "call doserrno_ptr" "db 137" "db 48" "db 129" "db 125" "db 16" "db 0" "db 1" "db 0" "db 0" "db 115" "db 57" "db 128" "db 61" "db 226" "db 194" "db 0" "db 0" "db 3" "db 114" "db 25" "db 128" "db 251" "db 80" "db 117" "db 4" "db 179" "db 14" "db 235" "db 16" "db 128" "db 251" "db 34" "db 114" "db 4" "db 179" "db 19" "db 235" "db 7" "db 128" "db 251" "db 32" "db 114" "db 2" "db 179" "db 5" "db 128" "db 251" "db 19" "db 118" "db 2" "db 179" "db 19" "db 15" "db 182" "db 195" "db 102" "db 15" "db 190" "db 128" "db 20" "db 192" "db 0" "db 0" "db 15" "db 191" "db 216" "db 235" "db 12" "db 139" "db 93" "db 16" "db 193" "db 235" "db 8" "db 129" "db 227" "db 255" "db 0" "db 0" "db 0" "call errno_ptr" "db 137" "db 24" "db 184" "db 255" "db 255" "db 255" "db 255" parm [] value [eax] modify exact [eax ebx esi];

int set_doserr(int code)
{
    return __doserr();
}
