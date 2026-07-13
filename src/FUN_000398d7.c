/* FUN_000398d7 -- SKIP-ASM-IDIOM (load instrument into a free slot, 143B).
 * disasm: framed; pushfd; cli;
 *   scan g_bcfa[0..15] for a free slot; if none return -1;
 *   if (record->magic (dword @ [ebp+8]+4) != 'Copy' 0x79706f43) return -1;
 *   g_bcfa[slot] = record->data;
 *   blk = FUN_00039994(slot); if (!blk) return -1;
 *   if (blk->size (dword @ blk) > g_bdcc) return -1;
 *   return slot;
 *   <restore-IF>; leave; ret
 * Body is C; PUSHFD/CLI + saved-flags IF-restore are asm. Parked. */
