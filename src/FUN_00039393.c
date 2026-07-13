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
