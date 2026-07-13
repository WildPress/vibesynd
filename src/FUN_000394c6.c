/* FUN_000394c6 -- SKIP-ASM-IDIOM (recompute minimum timer period, 134B).
 * disasm: pushfd; cli; cld; g_bcf2 = -1;
 *   for (i=0; i<=0x10; i++) if (g_bc38[i]) and g_bc9e[i] < g_bcf2 keep it;
 *   if (min != g_bce2) { g_bcee=-1; g_bce2=min; FUN_00039495(min);
 *                        rep stosd : clear g_bc5a (0x11 dwords); }
 *   <restore-IF>; popfd; pop es;edi;esi; ret
 * The scan/min is ordinary C but PUSHFD/CLI, REP STOSD and the saved-flags
 * IF-restore epilogue are asm. Parked (interrupt-critical-section idiom). */
