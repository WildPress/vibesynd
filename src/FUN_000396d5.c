/* FUN_000396d5 -- SKIP-ASM-IDIOM (stop / free a voice, 77B).
 * disasm: framed; pushfd; cli; id = [ebp+8];
 *   if (id != -1 && g_bc38[id] active) {
 *       g_bc38[id] = 0;
 *       if (--g_bbf0 == 0) { FUN_00039467(0); FUN_0003942f(); }  ; last voice:
 *   }                                                    ; stop PIT, restore ISR
 *   <restore-IF>; leave; ret
 * Refcount/teardown logic is C; PUSHFD/CLI + saved-flags IF-restore are asm.
 * Parked. */
