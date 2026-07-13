/* FUN_000397f1 -- SKIP-ASM-IDIOM (set voice period + recompute, 133B).
 * disasm: framed; pushfd; cli; id = [ebp+8];
 *   save old = g_bc38[id]; g_bc38[id] = 1;             ; mark active
 *   g_bc9e[id] = [ebp+0xc];                            ; period
 *   g_bc5a[id] = 0;
 *   FUN_000394c6();                                    ; recompute min period
 *   g_bc38[id] = old;                                  ; restore state
 *   <restore-IF>; leave; ret
 * Body is C; PUSHFD/CLI + saved-flags IF-restore are asm. Parked. */
