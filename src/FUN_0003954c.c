/* FUN_0003954c -- SKIP-ASM-IDIOM (init voice tables, 106B).
 * disasm: pushfd; cli; g_bdca = ds; es = g_bdca; g_bbf0 = g_bbf2 = 0; cld;
 *   rep stosd : zeros @ 0xbcfa (0x10 dwords)
 *   rep stosd : -1    @ 0xbd3a (0x10 dwords)
 *   rep stosd : zeros @ 0xbd7a (0x10 dwords)
 *   <restore-IF>; popfd; pop es;edi;esi; ret
 * ES setup + REP STOSD table inits + interrupt gating. Pure asm; parked. */
