/* FUN_0003954c -- SKIP-ASM-IDIOM (init voice tables, 106B).
 * disasm: pushfd; cli; g_bdca = ds; es = g_bdca; g_bbf0 = g_bbf2 = 0; cld;
 *   rep stosd : zeros @ 0xbcfa (0x10 dwords)
 *   rep stosd : -1    @ 0xbd3a (0x10 dwords)
 *   rep stosd : zeros @ 0xbd7a (0x10 dwords)
 *   <restore-IF>; popfd; pop es;edi;esi; ret
 * ES setup + REP STOSD table inits + interrupt gating. Pure asm; parked. */
extern void __b0003954c1(void);
extern void __b0003954c2(void);
#pragma aux __b0003954c1 = "db 86" "db 87" "db 6" "db 156" "db 250" "db 102" "db 140" "db 29" "db 202" "db 189" "db 0" "db 0" "db 102" "db 142" "db 5" "db 202" "db 189" "db 0" "db 0" "db 102" "db 199" "db 5" "db 240" "db 187" "db 0" "db 0" "db 0" "db 0" "db 102" "db 199" "db 5" "db 242" "db 187" "db 0" "db 0" "db 0" "db 0" "db 252" "db 191" "db 250" "db 188" "db 0" "db 0" "db 185" "db 16" "db 0" "db 0" "db 0" "db 184" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __b0003954c2 = "db 243" "db 171" "db 191" "db 58" "db 189" "db 0" "db 0" "db 185" "db 16" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 243" "db 171" "db 191" "db 122" "db 189" "db 0" "db 0" "db 185" "db 16" "db 0" "db 0" "db 0" "db 184" "db 0" "db 0" "db 0" "db 0" "db 243" "db 171" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 7" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003954c modify [eax ebx ecx edx esi edi ebp];
void FUN_0003954c(void) { __b0003954c1(); __b0003954c2(); }
