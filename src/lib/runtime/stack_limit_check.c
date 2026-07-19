/* C runtime: __STK stack-limit check core @ 0x3cade (CLIB3S, Watcom 9.5). Given a
   proposed new stack top in EAX: if EAX >= ESP it is fine (JNC ret); otherwise the
   requested depth (ESP-EAX) is compared against the stack-limit global g_c2bc and, if
   within limit, returns; if the limit is exceeded it re-checks that SS still matches the
   stored stack selector g_c294 (JZ falls through to the overflow reporter 0x3cafb) and
   otherwise returns. Frameless hand-asm with two internal RETs -> whole body
   db-transcribed into a frameless wrapper; both abs32 globals are literal db bytes; the
   final RET is supplied by the wrapper (the internal RET at +0x10 is kept). */
extern void __cade(void);
#pragma aux __cade = "db 59" "db 196" "db 115" "db 13" "db 43" "db 196" "db 247" "db 216" "db 59" "db 5" "db 188" "db 194" "db 0" "db 0" "db 118" "db 1" "db 195" "db 140" "db 208" "db 102" "db 59" "db 5" "db 148" "db 194" "db 0" "db 0" "db 116" "db 1" modify exact [eax];
#pragma aux stack_limit_check modify [eax];
void stack_limit_check(void)
{
    __cade();
}
