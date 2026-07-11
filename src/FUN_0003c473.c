/* Decomp target #2  -  original @ 0x0003c473 (6 bytes)
 *
 *   Original machine code:  B8 40 1E 01 00 C3
 *   Disassembly:            MOV EAX, 0x11e40
 *                           RET
 *
 * Sibling of FUN_0003c46d (+4). Accessor returning &global @ 0x11e40; callers
 * write through it (e.g. FUN_0003c4b9 does *(uint*)FUN_0003c473() = key & 0xff).
 * Not relocated (verified via tools/le_fixups.py) -> literal absolute address.
 */
void *FUN_0003c473(void)
{
    return (void *)0x11e40;
}
