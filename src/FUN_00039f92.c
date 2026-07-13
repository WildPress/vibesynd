/* FUN_00039f92 -- SKIP-ASM-IDIOM (VGA palette DAC upload, 161B).
 * disasm: push esi; g_be2e = 0; if (g_bdf4 != 1) return;
 *   decode a run-length RGB palette from g_10ab0 into the DAC buffer g_10aa8
 *   (per entry: count, then <count> RGB triples, blanks expanded);
 *   out 0x3c8, 0                    ; DAC write index = 0
 *   INT 10h / AX=1201h              ; enable palette loading
 *   wait: in al,0x3da; test al,8; jz wait   ; vertical-retrace poll
 *   rep outsb -> port 0x3c9 (0x300 bytes)   ; upload the DAC data
 *   pop esi; ret
 * OUT/IN port I/O, INT 10h and REP OUTSB are pure asm; parked. */
