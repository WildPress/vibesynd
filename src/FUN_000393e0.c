/* FUN_000393e0 -- SKIP-ASM-IDIOM (install timer ISR, 79B).
 * disasm: pushfd; cli;
 *   INT 21h / AH=35h : get interrupt vector 8 -> save offset g_bce6, seg g_bcea
 *   g_bc34 = 0x2bc2b;
 *   INT 21h / AH=25h : set vector 8 = CS:0x2bb76 (new timer ISR, via mov ds,cs)
 *   <restore-IF>; popfd; pop edi;esi; ret
 * DOS get/set-interrupt-vector through INT 21h plus segment-register juggling
 * (push ds; mov ds,bx; ...; pop ds). Pure asm; parked. */
