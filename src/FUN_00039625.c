/* FUN_00039625 -- SKIP-ASM-IDIOM (allocate a voice slot, 176B).
 * disasm: framed; pushfd; cli;
 *   scan g_bc38[0..15] for a free slot; if none return -1;
 *   mark slot active (=1), store the sound pointer [ebp+8] into g_bbf4[slot],
 *   reset its period g_bc9e[slot] = -1;
 *   if (++g_bbf0 == 1) {   ; first active voice -> bring up the driver
 *       FUN_00039393(slot); g_bc58 = 1; FUN_000393e0();
 *       FUN_000397f1(0x10, 0xd68d); FUN_00039747(0x10);
 *       arm slot 0x10;
 *   }
 *   <restore-IF>; leave; ret; returns slot index
 * Allocation logic is C; PUSHFD/CLI + saved-flags IF-restore are asm. Parked. */
