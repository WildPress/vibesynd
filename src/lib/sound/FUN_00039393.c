/* FUN_00039393 -- SKIP-ASM-IDIOM (clear voice tables, 77B).
 * disasm: push esi;edi;es; pushfd; cli; push ds; pop es; cld;
 *   g_bce2 = -1;
 *   rep stosw : 0x11 zero words  @ 0xbc38
 *   rep stosd : 0x11 zero dwords @ 0xbc5a
 *   rep stosd : 0x11 zero dwords @ 0xbc9e
 *   <inlined restore-IF from saved EFLAGS>; popfd; pop es;edi;esi; ret
 * PUSHFD/CLI, the ES load and the REP STOSW/STOSD table clears plus the
 * "restore interrupt-enable from saved flags" epilogue are pure x86 that
 * headerless Watcom C cannot emit. Parked (interrupt-critical-section idiom). */
extern void __b000393931(void);
extern void __b000393932(void);
#pragma aux __b000393931 = "db 86" "db 87" "db 6" "db 156" "db 250" "db 30" "db 7" "db 252" "db 199" "db 5" "db 226" "db 188" "db 0" "db 0" "db 255" "db 255" "db 255" "db 255" "db 191" "db 56" "db 188" "db 0" "db 0" "db 185" "db 17" "db 0" "db 0" "db 0" "db 184" "db 0" "db 0" "db 0" "db 0" "db 243" "db 102" "db 171" "db 191" "db 90" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __b000393932 = "db 188" "db 0" "db 0" "db 185" "db 17" "db 0" "db 0" "db 0" "db 243" "db 171" "db 191" "db 158" "db 188" "db 0" "db 0" "db 185" "db 17" "db 0" "db 0" "db 0" "db 243" "db 171" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 7" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00039393 modify [eax ebx ecx edx esi edi ebp];
void FUN_00039393(void) { __b000393931(); __b000393932(); }
