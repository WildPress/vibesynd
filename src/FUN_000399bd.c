/* FUN_000399bd -- SKIP-ASM-IDIOM (start a note on a voice, 197B).
 * disasm: framed; pushfd; cli; id = [ebp+8]; if (id >= 0x10) goto done;
 *   g_bdc6 = -1; inst = FUN_00039994(id);
 *   if (inst->[0x20] != -1) {
 *       h = FUN_00039280(0x67, id);
 *       if (h) { v = FUN_00039625(h); g_bd3a[id] = v; g_bdc6 = v;
 *                FUN_00039846(g_bdc6, inst->[0x20]); }
 *   }
 *   FUN_000392ac(0x66, id, [ebp+0xc], [ebp+0x10], [ebp+0x14], [ebp+0x18]);
 *   g_bd7a[id] = 1;
 *   if (g_bdc6 != -1) FUN_00039747(g_bdc6);
 *   done: <restore-IF>; leave; ret
 * Dispatch logic is C; PUSHFD/CLI + saved-flags IF-restore plus the computed
 * key-record dispatch through FUN_000392ac are asm. Parked. */
