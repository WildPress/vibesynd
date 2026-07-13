/* FUN_0003979c -- SKIP-ASM-IDIOM (voice state 2 -> 1, 48B).
 * disasm: framed; pushfd; cli; id = [ebp+8];
 *   if (g_bc38[id] == 2) g_bc38[id] = 1;
 *   <restore-IF>; leave; ret
 * A trivial flag flip wrapped in PUSHFD/CLI plus the saved-flags IF-restore
 * epilogue; the asm bracket is not reachable from headerless Watcom C. Parked. */
