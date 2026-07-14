/* install_timer_isr -- SKIP-ASM-IDIOM (install timer ISR, 79B).
 * disasm: pushfd; cli;
 *   INT 21h / AH=35h : get interrupt vector 8 -> save offset g_bce6, seg g_bcea
 *   g_bc34 = 0x2bc2b;
 *   INT 21h / AH=25h : set vector 8 = CS:0x2bb76 (new timer ISR, via mov ds,cs)
 *   <restore-IF>; popfd; pop edi;esi; ret
 * DOS get/set-interrupt-vector through INT 21h plus segment-register juggling
 * (push ds; mov ds,bx; ...; pop ds). Pure asm; parked. */
extern void __b000393e01(void);
extern void __b000393e02(void);
#pragma aux __b000393e01 = "db 86" "db 87" "db 156" "db 250" "db 184" "db 8" "db 0" "db 0" "db 0" "db 180" "db 53" "db 6" "db 205" "db 33" "db 102" "db 140" "db 194" "db 7" "db 137" "db 29" "db 230" "db 188" "db 0" "db 0" "db 137" "db 21" "db 234" "db 188" "db 0" "db 0" "db 187" "db 43" "db 188" "db 2" "db 0" "db 137" "db 29" "db 52" "db 188" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __b000393e02 = "db 0" "db 0" "db 184" "db 8" "db 0" "db 0" "db 0" "db 186" "db 118" "db 187" "db 2" "db 0" "db 102" "db 140" "db 203" "db 180" "db 37" "db 30" "db 102" "db 142" "db 219" "db 205" "db 33" "db 31" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux install_timer_isr modify [eax ebx ecx edx esi edi ebp];
void install_timer_isr(void) { __b000393e01(); __b000393e02(); }
