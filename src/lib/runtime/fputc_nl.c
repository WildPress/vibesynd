/* C runtime helper @ 0x3b2b5 (CLIB3S region, buffered console output). Writes the
   character c to the console stream (FILE at 0xc09a) via the fast putc fputs;
   on success it emits a newline: if the stream is in the special mode (flag 0xc0a6 &
   0x40) and the buffer has room ((g_c0ae - g_c09e) > 1) it inlines the putc (set the
   0x10 flag bit at 0xc0a7, bump the count at 0xc09e, store 0x0A into *_ptr++),
   otherwise it calls the slow putc fputc(0x0A, stream). Returns 0 when the
   newline write succeeded (result 0x0A), the write result otherwise, and -1 if the
   first write failed.
     Plain C compiles cleanly except for a register-role/codegen cascade: the target
   homes the return value in EDX (mov eax,edx at the single exit) and increments the
   buffer pointer with `inc [mem]`, where our Watcom 9.5b uses EAX + `lea` -- an
   accumulator-selection tie (playbook wall class 2). As this is RTL library code it is
   whole-function db-transcribed instead, split across two consecutive #pragma aux
   routines for the ~1024-char DOS source-line limit; the two putc calls are masked
   relocs, the fixed FILE-field addresses are literal bytes, and the trailing RET is
   supplied by the wrapper. */
extern void fputs(void);
extern void fputc(void);
extern void __putln1(void);
extern int  __putln2(void);
#pragma aux __putln1 = "db 85" "db 137" "db 229" "db 104" "db 154" "db 192" "db 0" "db 0" "db 255" "db 117" "db 8" "call fputs" "db 131" "db 196" "db 8" "db 137" "db 194" "db 131" "db 248" "db 255" "db 116" "db 83" "db 246" "db 5" "db 166" "db 192" "db 0" "db 0" "db 64" "db 116" "db 50" "db 161" "db 174" "db 192" "db 0" "db 0" "db 43" "db 5" "db 158" "db 192" "db 0" "db 0" "db 131" "db 248" "db 1" "db 118" "db 34" "db 128" "db 13" "db 167" "db 192" "db 0" "db 0" "db 16" parm [] modify exact [eax ecx edx];
#pragma aux __putln2 = "db 255" "db 5" "db 158" "db 192" "db 0" "db 0" "db 161" "db 154" "db 192" "db 0" "db 0" "db 255" "db 5" "db 154" "db 192" "db 0" "db 0" "db 198" "db 0" "db 10" "db 184" "db 10" "db 0" "db 0" "db 0" "db 235" "db 15" "db 104" "db 154" "db 192" "db 0" "db 0" "db 106" "db 10" "call fputc" "db 131" "db 196" "db 8" "db 137" "db 194" "db 131" "db 248" "db 10" "db 117" "db 2" "db 49" "db 210" "db 137" "db 208" "db 93" parm [] value [eax] modify exact [eax ecx edx];
int fputc_nl(int c)
{
    __putln1();
    return __putln2();
}
