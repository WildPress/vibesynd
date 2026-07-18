/* uninstall_timer_isr -- SKIP-ASM-IDIOM (restore timer ISR, 56B).
 * disasm: pushfd; cli; g_bcee = -1;
 *   INT 21h / AH=25h : set interrupt vector 8 back to the saved handler
 *                      (offset g_bce6, segment g_bcea via mov ds,bx)
 *   <restore-IF>; popfd; pop edi;esi; ret
 * DOS set-interrupt-vector via INT 21h + segment juggling. Pure asm; parked. */
extern void __b0003942f(void);
#pragma aux __b0003942f = "db 86" "db 87" "db 156" "db 250" "db 199" "db 5" "db 238" "db 188" "db 0" "db 0" "db 255" "db 255" "db 255" "db 255" "db 184" "db 8" "db 0" "db 0" "db 0" "db 139" "db 21" "db 230" "db 188" "db 0" "db 0" "db 139" "db 29" "db 234" "db 188" "db 0" "db 0" "db 180" "db 37" "db 30" "db 102" "db 142" "db 219" "db 205" "db 33" "db 31" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux uninstall_timer_isr modify [eax ebx ecx edx esi edi ebp];
void uninstall_timer_isr(void) { __b0003942f(); }
