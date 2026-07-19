/* C runtime: __STK stack-probe accessor @ 0x3cabb (CLIB3S, Watcom 9.5). Returns
   ESP - g_c2bc (the current stack depth below the stack-limit global) in EAX.
   Frameless hand-asm (mov eax,esp; sub eax,[0xc2bc]; ret). Emitted as a frameless
   wrapper whose whole body is the inlined db pragma reading ESP directly; the
   absolute ref 0xc2bc is a literal db abs32; trailing RET supplied by the wrapper. */
extern unsigned __cabb(void);
#pragma aux __cabb = "db 137" "db 224" "db 43" "db 5" "db 188" "db 194" "db 0" "db 0" value [eax] modify exact [eax];
unsigned stack_avail(void)
{
    return __cabb();
}
