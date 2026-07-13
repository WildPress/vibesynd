/* FUN_0003942f -- SKIP-ASM-IDIOM (restore timer ISR, 56B).
 * disasm: pushfd; cli; g_bcee = -1;
 *   INT 21h / AH=25h : set interrupt vector 8 back to the saved handler
 *                      (offset g_bce6, segment g_bcea via mov ds,bx)
 *   <restore-IF>; popfd; pop edi;esi; ret
 * DOS set-interrupt-vector via INT 21h + segment juggling. Pure asm; parked. */
