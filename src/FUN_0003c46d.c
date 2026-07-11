/* Decomp target #1  -  original @ 0x0003c46d (6 bytes)
 *
 *   Original machine code:  B8 3C 1E 01 00 C3
 *   Disassembly:            MOV EAX, 0x11e3c
 *                           RET
 *
 * A tiny accessor (called from 20 sites) that returns the constant 0x11e3c.
 * In the shipped game this is the address of a global; in our un-relocated
 * segment it is baked in as a literal, so returning the literal reproduces
 * the exact bytes. Return value goes in EAX per Watcom's register convention.
 *
 * Success = wcc386 emits these same 6 bytes (verified by tools/match.py).
 */
void *FUN_0003c46d(void)
{
    return (void *)0x11e3c;
}
