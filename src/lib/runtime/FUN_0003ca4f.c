/* C runtime: aligned dword-fill core @ 0x3ca4f (CLIB3S, Watcom 9.5). The inner
   loop of memset: fills ECX dwords of pattern EDX starting at EAX, aligning to a
   0x20 boundary first (store dword + lea eax,[eax+4] until AL&0x1f==0), then a
   16-bytes-per-iteration unrolled body (with a self-modifying-style DEC/JNZ ladder
   and a peek "cmp [eax+0x20],dl" prefetch touch), finishing the 0..3 dword tail.
   Leaf hand-asm, register in/out (eax=dst, edx=pattern, ecx=count) -> frameless
   void wrapper, whole body db-transcribed, trailing RET supplied by the wrapper. */
extern void __ca4f(void);
#pragma aux __ca4f = "db 11" "db 201" "db 116" "db 103" "db 168" "db 31" "db 116" "db 8" "db 137" "db 16" "db 141" "db 64" "db 4" "db 73" "db 117" "db 244" "db 81" "db 193" "db 233" "db 2" "db 116" "db 58" "db 73" "db 116" "db 41" "db 137" "db 16" "db 137" "db 80" "db 4" "db 73" "db 137" "db 80" "db 8" "db 137" "db 80" "db 12" "db 116" "db 24" "db 56" "db 80" "db 32" "db 137" "db 80" "db 16" "db 137" "db 80" "db 20" "db 73" "db 137" "db 80" "db 24" "db 137" "db 80" "db 28" "db 141" "db 64" "db 32" "db 117" "db 221" "db 141" "db 64" "db 240" "db 141" "db 64" "db 16" "db 137" "db 16" "db 137" "db 80" "db 4" "db 137" "db 80" "db 8" "db 137" "db 80" "db 12" "db 141" "db 64" "db 16" "db 89" "db 131" "db 225" "db 3" "db 116" "db 21" "db 137" "db 16" "db 141" "db 64" "db 4" "db 73" "db 116" "db 13" "db 137" "db 16" "db 141" "db 64" "db 4" "db 73" "db 116" "db 5" "db 137" "db 16" "db 141" "db 64" "db 4" modify exact [eax ecx edx];
void FUN_0003ca4f(void)
{
    __ca4f();
}
