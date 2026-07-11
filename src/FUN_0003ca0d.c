/* Decomp target #3  -  original @ 0x0003ca0d (7 bytes)
 *
 *   Original machine code:  55 89 E5 31 C0 5D C3
 *   Disassembly:            PUSH EBP ; MOV EBP,ESP ; XOR EAX,EAX ; POP EBP ; RET
 *
 * Just `return 0;` but built WITH a stack frame. Our default WATFLAGS emit
 * frameless code (31 C0 C3), so this is a flag-calibration target: find the
 * Watcom flags that produce the push ebp/mov ebp,esp/pop ebp frame.
 */
int FUN_0003ca0d(void)
{
    return 0;
}
