/* FUN_00039467 -- SKIP-ASM-IDIOM (reprogram PIT channel 0, 46B).
 * disasm: framed; pushfd; cli;
 *   out 0x43, 0x36                 ; counter 0, lobyte/hibyte, mode 3, binary
 *   eax = [ebp+8]; g_bcf6 = eax    ; latch requested divisor
 *   out 0x40, al ; out 0x40, ah    ; program the reload value
 *   <restore-IF>; leave; ret
 * Direct 8253/8254 timer programming via OUT 0x43/0x40 plus interrupt gating.
 * Pure asm; parked. */
