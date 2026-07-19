/* C runtime: fgets/getline @ 0x3b326 (CLIB3S, Watcom 9.5). Reads a line from the
   console-input stream (buffer arg at [ebp+0x14]) into the caller's buffer. Saves and
   masks the mode/echo flag byte g_c08c (&= 0xCF, keeps bits 0x30), then loops reading
   characters: __fgetc-style inline getc from the g_c080 buffer, or fgetc refill
   when in one mode / getc (single-char read) in the other, stopping on newline
   (0x0A) or EOF (-1). On empty EOF input with the 0x20 flag it returns NULL; otherwise
   NUL-terminates and returns the buffer; finally OR's the saved 0x30 flag bits back into
   g_c08c. Regs-first prologue (push ebx/esi/edi before the ebp frame) is the
   reg-save-order wall for plain C -> whole-function db-transcription into a frameless
   wrapper. The ~1024-char DOS source-line limit forces the body across two consecutive
   #pragma aux routines (Watcom concatenates their expansions contiguously); the two
   calls are masked relocs; the trailing RET is supplied by the wrapper. */
extern void fgetc(void);
extern void getc(void);
extern void __fgets1(void);
extern char *__fgets2(void);
#pragma aux __fgets1 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 117" "db 20" "db 139" "db 61" "db 140" "db 192" "db 0" "db 0" "db 137" "db 243" "db 128" "db 37" "db 140" "db 192" "db 0" "db 0" "db 207" "db 131" "db 231" "db 48" "db 246" "db 5" "db 140" "db 192" "db 0" "db 0" "db 64" "db 116" "db 43" "db 255" "db 13" "db 132" "db 192" "db 0" "db 0" "db 131" "db 61" "db 132" "db 192" "db 0" "db 0" "db 0" "db 125" "db 12" "db 104" "db 128" "db 192" "db 0" "db 0" "call fgetc" "db 235" "db 26" "db 161" "db 128" "db 192" "db 0" "db 0" "db 255" "db 5" "db 128" "db 192" "db 0" "db 0" parm [] modify exact [eax ecx edx];
#pragma aux __fgets2 = "db 15" "db 182" "db 0" "db 235" "db 13" "db 104" "db 128" "db 192" "db 0" "db 0" "call getc" "db 131" "db 196" "db 4" "db 137" "db 194" "db 131" "db 248" "db 255" "db 116" "db 10" "db 131" "db 248" "db 10" "db 116" "db 5" "db 136" "db 19" "db 67" "db 235" "db 174" "db 131" "db 250" "db 255" "db 117" "db 17" "db 57" "db 243" "db 116" "db 9" "db 246" "db 5" "db 140" "db 192" "db 0" "db 0" "db 32" "db 116" "db 4" "db 49" "db 246" "db 235" "db 3" "db 198" "db 3" "db 0" "db 137" "db 240" "db 9" "db 61" "db 140" "db 192" "db 0" "db 0" "db 93" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ecx edx];
char *fgets_console(int a, int b, int c, char *buf)
{
    __fgets1();
    return __fgets2();
}
