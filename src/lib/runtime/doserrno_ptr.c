/* Decomp target #2  -  original @ 0x0003c473 (6 bytes)
 *
 *   Original machine code:  B8 40 1E 01 00 C3
 *   Disassembly:            MOV EAX, 0x11e40
 *                           RET
 *
 * Sibling of errno_ptr (+4). Accessor returning &global @ 0x11e40; callers
 * write through it (e.g. set_doserr does *(uint*)doserrno_ptr() = key & 0xff).
 * Not relocated (verified via tools/le_fixups.py) -> literal absolute address.
 */
void *doserrno_ptr(void)
{
    return (void *)0x11e40;
}
