/* C runtime: __STK stack-overflow reporter @ 0x3cafb (CLIB3S, Watcom 9.5). Loads the
   stack-error message-table address 0xc296 into EAX and the code 1 into EDX, then
   tail-jumps to the runtime fatal-error handler FUN_0003b1d3 (never returns). Emitted
   as a register-argument tail-call: FUN_0003b1d3 is declared with parm [eax][edx] and
   `aborts`, so Watcom loads eax/edx and JMP-tail-calls it; both wrapper and callee are
   `aborts` so no call/ret framing is added. The 0xc296 immediate is a literal abs32. */
extern void FUN_0003b1d3(unsigned a, int b);
#pragma aux FUN_0003b1d3 parm [eax] [edx] aborts;
#pragma aux FUN_0003cafb aborts modify [eax edx];
void FUN_0003cafb(void)
{
    FUN_0003b1d3(0xc296u, 1);
}
