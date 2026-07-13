/* FUN_000395b6 -- SKIP-ASM-IDIOM (per-voice service loop, 111B).
 * disasm: framed; pushfd; cli;
 *   for (i=0; i<0x10; i++) if (g_bcfa[i]) {
 *       if (g_bd3a[i] != -1) FUN_000396d5(g_bd3a[i]);
 *       FUN_00039a82(i, [ebp+8]);
 *   }
 *   FUN_00039722();
 *   <restore-IF>; leave; ret
 * The loop body is ordinary C but the surrounding PUSHFD/CLI and the saved-flags
 * IF-restore epilogue are asm. Parked (interrupt-critical-section idiom). */
