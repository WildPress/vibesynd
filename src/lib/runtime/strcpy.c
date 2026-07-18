/* C runtime: strcpy @ 0x3a8d7 (CLIB3S). Open Watcom strcpy.c uses #pragma aux __strcpy
   (hand asm, 2-byte unroll) for 386 small model. Replicated verbatim; the target encodes
   `add eax,2` as the EAX-accumulator form (05 02000000), emitted here as raw db bytes. */
extern char *__strcpy(char *s, const char *t);
#pragma aux __strcpy = "push eax" "L1: mov cl,[edx]" "mov [eax],cl" "cmp cl,0" "je short L2" "mov cl,1[edx]" "add edx,2" "mov 1[eax],cl" "db 5" "db 2" "db 0" "db 0" "db 0" "cmp cl,0" "jne short L1" "L2: pop eax" parm [eax] [edx] value [eax] modify exact [ecx edx];
char *strcpy(char *s, const char *t)
{
    return __strcpy(s, t);
}
