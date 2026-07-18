/* recompute_timer_period -- SKIP-ASM-IDIOM (recompute minimum timer period, 134B).
 * disasm: pushfd; cli; cld; g_bcf2 = -1;
 *   for (i=0; i<=0x10; i++) if (g_bc38[i]) and g_bc9e[i] < g_bcf2 keep it;
 *   if (min != g_bce2) { g_bcee=-1; g_bce2=min; program_pit_period_us(min);
 *                        rep stosd : clear g_bc5a (0x11 dwords); }
 *   <restore-IF>; popfd; pop es;edi;esi; ret
 * The scan/min is ordinary C but PUSHFD/CLI, REP STOSD and the saved-flags
 * IF-restore epilogue are asm. Parked (interrupt-critical-section idiom). */
extern void __b000394c61(void);
extern void __b000394c62(void);
#pragma aux __b000394c61 = "db 86" "db 87" "db 6" "db 156" "db 250" "db 252" "db 199" "db 5" "db 242" "db 188" "db 0" "db 0" "db 255" "db 255" "db 255" "db 255" "db 190" "db 0" "db 0" "db 0" "db 0" "db 139" "db 222" "db 209" "db 227" "db 102" "db 131" "db 187" "db 56" "db 188" "db 0" "db 0" "db 0" "db 116" "db 20" "db 139" "db 4" "db 93" "db 158" "db 188" "db 0" "db 0" "db 59" "db 5" "db 242" "db 188" "db 0" "db 0" "db 115" "db 5" "db 163" "db 242" "db 188" "db 0" "db 0" "db 70" "db 131" "db 254" "db 16" "db 118" "db 216" "db 161" "db 242" "db 188" "db 0" "db 0" "db 59" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __b000394c62 = "db 5" "db 226" "db 188" "db 0" "db 0" "db 116" "db 43" "db 199" "db 5" "db 238" "db 188" "db 0" "db 0" "db 255" "db 255" "db 255" "db 255" "db 163" "db 226" "db 188" "db 0" "db 0" "db 80" "db 232" "db 112" "db 255" "db 255" "db 255" "db 131" "db 196" "db 4" "db 191" "db 90" "db 188" "db 0" "db 0" "db 185" "db 17" "db 0" "db 0" "db 0" "db 184" "db 0" "db 0" "db 0" "db 0" "db 30" "db 7" "db 243" "db 171" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 7" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux recompute_timer_period modify [eax ebx ecx edx esi edi ebp];
void recompute_timer_period(void) { __b000394c61(); __b000394c62(); }
