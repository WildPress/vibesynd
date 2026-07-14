/* FUN_000395b6 -- SKIP-ASM-IDIOM (per-voice service loop, 111B).
 * disasm: framed; pushfd; cli;
 *   for (i=0; i<0x10; i++) if (g_bcfa[i]) {
 *       if (g_bd3a[i] != -1) FUN_000396d5(g_bd3a[i]);
 *       FUN_00039a82(i, [ebp+8]);
 *   }
 *   FUN_00039722();
 *   <restore-IF>; leave; ret
 * The loop body is ordinary C but the surrounding PUSHFD/CLI and the saved-flags
 * IF-restore epilogue are asm. Parked (interrupt-critical-section idiom). */
extern void __b000395b61(void);
extern void __b000395b62(void);
#pragma aux __b000395b61 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 199" "db 5" "db 190" "db 189" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 139" "db 53" "db 190" "db 189" "db 0" "db 0" "db 193" "db 230" "db 2" "db 139" "db 150" "db 58" "db 189" "db 0" "db 0" "db 139" "db 134" "db 250" "db 188" "db 0" "db 0" "db 131" "db 248" "db 0" "db 116" "db 31" "db 131" "db 250" "db 255" "db 116" "db 9" "db 82" "db 232" "db 233" "db 0" "db 0" "db 0" "db 131" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __b000395b62 = "db 196" "db 4" "db 255" "db 117" "db 8" "db 255" "db 53" "db 190" "db 189" "db 0" "db 0" "db 232" "db 133" "db 4" "db 0" "db 0" "db 131" "db 196" "db 8" "db 255" "db 5" "db 190" "db 189" "db 0" "db 0" "db 131" "db 61" "db 190" "db 189" "db 0" "db 0" "db 16" "db 117" "db 184" "db 232" "db 14" "db 1" "db 0" "db 0" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_000395b6 modify [eax ebx ecx edx esi edi ebp];
void FUN_000395b6(void) { __b000395b61(); __b000395b62(); }
