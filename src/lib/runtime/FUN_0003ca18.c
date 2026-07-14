/* C runtime: byte-fill core @ 0x3ca18 (CLIB3S, Watcom 9.5). The byte-granular
   front of memset: if ECX!=0, aligns EAX to a 4-byte boundary storing DL and
   rotating the pattern EDX right by 8 each step, then calls the aligned dword-fill
   core FUN_0003ca4f for ECX>>2 dwords, and stores the 0..3 trailing bytes (DL,DH,DL).
   Register in/out (eax=dst, edx=pattern, ecx=count) -> frameless void wrapper, whole
   body db-transcribed with a real masked `call FUN_0003ca4f`; trailing RET from wrapper. */
extern void FUN_0003ca4f(void);
extern void __ca18(void);
#pragma aux __ca18 = "db 11" "db 201" "db 116" "db 44" "db 56" "db 16" "db 168" "db 3" "db 116" "db 9" "db 136" "db 16" "db 64" "db 193" "db 202" "db 8" "db 73" "db 117" "db 243" "db 81" "db 193" "db 233" "db 2" "call FUN_0003ca4f" "db 89" "db 131" "db 225" "db 3" "db 116" "db 14" "db 136" "db 16" "db 73" "db 116" "db 9" "db 136" "db 112" "db 1" "db 73" "db 116" "db 3" "db 136" "db 80" "db 2" modify exact [eax ecx edx];
void FUN_0003ca18(void)
{
    __ca18();
}
