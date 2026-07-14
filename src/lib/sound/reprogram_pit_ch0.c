/* reprogram_pit_ch0 -- SKIP-ASM-IDIOM (reprogram PIT channel 0, 46B).
 * disasm: framed; pushfd; cli;
 *   out 0x43, 0x36                 ; counter 0, lobyte/hibyte, mode 3, binary
 *   eax = [ebp+8]; g_bcf6 = eax    ; latch requested divisor
 *   out 0x40, al ; out 0x40, ah    ; program the reload value
 *   <restore-IF>; leave; ret
 * Direct 8253/8254 timer programming via OUT 0x43/0x40 plus interrupt gating.
 * Pure asm; parked. */
extern void __b00039467(void);
#pragma aux __b00039467 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 176" "db 54" "db 230" "db 67" "db 139" "db 69" "db 8" "db 163" "db 246" "db 188" "db 0" "db 0" "db 235" "db 0" "db 230" "db 64" "db 138" "db 196" "db 235" "db 0" "db 230" "db 64" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux reprogram_pit_ch0 modify [eax ebx ecx edx esi edi ebp];
void reprogram_pit_ch0(void) { __b00039467(); }
