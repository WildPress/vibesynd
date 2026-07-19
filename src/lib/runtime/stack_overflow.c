/* C runtime: __STK stack-overflow reporter @ 0x3cafb (CLIB3S, Watcom 9.5). Loads the
   stack-error message-table address 0xc296 into EAX and the code 1 into EDX, then
   tail-jumps to the runtime fatal-error handler write_msg_and_exit (never returns). Emitted
   as a register-argument tail-call: write_msg_and_exit is declared with parm [eax][edx] and
   `aborts`, so Watcom loads eax/edx and JMP-tail-calls it; both wrapper and callee are
   `aborts` so no call/ret framing is added. The 0xc296 immediate is a literal abs32. */
extern void write_msg_and_exit(unsigned a, int b);
#pragma aux write_msg_and_exit parm [eax] [edx] aborts;
#pragma aux stack_overflow aborts modify [eax edx];
void stack_overflow(void)
{
    write_msg_and_exit(0xc296u, 1);
}
