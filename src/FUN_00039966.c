/* FUN_00039966 -- SKIP-ASM-IDIOM (clear instrument slot, 46B).
 * disasm: framed; pushfd; cli; id = [ebp+8];
 *   if ((unsigned)id < 0x10) g_bcfa[id] = 0;
 *   <restore-IF>; leave; ret
 * A trivial store wrapped in PUSHFD/CLI plus the saved-flags IF-restore
 * epilogue; the asm bracket is not reachable from headerless Watcom C. Parked. */
