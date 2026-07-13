/* FUN_00039747 -- SKIP-ASM-IDIOM (voice state 1 -> 2, 48B).
 * disasm: framed; pushfd; cli; id = [ebp+8];
 *   if (g_bc38[id] == 1) g_bc38[id] = 2;
 *   <restore-IF>; leave; ret
 * A trivial flag flip wrapped in PUSHFD/CLI plus the saved-flags IF-restore
 * epilogue; the asm bracket is not reachable from headerless Watcom C. Parked. */
