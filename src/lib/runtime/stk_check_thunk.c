/* C runtime: __STK stack-check thunk @ 0x3cacb (CLIB3S, Watcom 9.5). Preserves the
   caller's EAX by XCHG'ing it with the size argument at [esp+4], calls the stack-limit
   check core stack_limit_check with the size in EAX, restores EAX from the slot, and returns
   popping its 4-byte arg (RET 4 = callee-cleanup). Frameless hand-asm; whole body
   db-transcribed INCLUDING the RET 4 (c2 04 00), so the wrapper is marked `aborts` to
   suppress its own return; the CALL rel32 to 0x3cade is a real masked extern. */
extern void stack_limit_check(void);
extern void __cacb(void);
#pragma aux __cacb = "db 135" "db 68" "db 36" "db 4" "call stack_limit_check" "db 139" "db 68" "db 36" "db 4" "db 194" "db 4" "db 0" aborts modify exact [eax ebx ecx edx];
#pragma aux stk_check_thunk aborts modify [eax ebx ecx edx];
void stk_check_thunk(void)
{
    __cacb();
}
