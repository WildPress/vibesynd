/* C runtime: fgetc (text mode) @ 0x3b539 (CLIB3S, Watcom 9.5). Reads one character
   from the stream (FILE* at [ebp+0xc]): inlined getc (--_cnt; if >=0 take *_ptr++ else
   __filbuf fgetc). On a CR (0x0D) it reads the following byte too (CR/LF text
   translation, discarding the CR). On the DOS EOF marker 0x1A it sets the EOF flag
   (_flag |= 0x10) and returns -1. Regs-first prologue (push ebx before the ebp frame)
   is the reg-save-order wall for plain C, so it is whole-function db-transcribed into a
   frameless wrapper; the two __filbuf calls are masked relocs; the trailing RET is
   supplied by the wrapper. */
extern void fgetc(void);
extern int __fgetc(void);
#pragma aux __fgetc = "db 83" "db 85" "db 137" "db 229" "db 139" "db 93" "db 12" "db 255" "db 75" "db 4" "db 131" "db 123" "db 4" "db 0" "db 125" "db 13" "db 83" "call fgetc" "db 131" "db 196" "db 4" "db 137" "db 194" "db 235" "db 9" "db 139" "db 3" "db 255" "db 3" "db 138" "db 0" "db 15" "db 182" "db 208" "db 137" "db 208" "db 131" "db 250" "db 13" "db 117" "db 29" "db 255" "db 75" "db 4" "db 131" "db 123" "db 4" "db 0" "db 125" "db 11" "db 83" "call fgetc" "db 131" "db 196" "db 4" "db 235" "db 9" "db 139" "db 3" "db 255" "db 3" "db 138" "db 0" "db 15" "db 182" "db 192" "db 131" "db 248" "db 26" "db 117" "db 9" "db 184" "db 255" "db 255" "db 255" "db 255" "db 128" "db 75" "db 12" "db 16" "db 93" "db 91" parm [] value [eax] modify exact [eax ecx edx];
int fread(void *fp)
{
    return __fgetc();
}
