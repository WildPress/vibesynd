/* FUN_00039879 -- SKIP-ASM-IDIOM (rate -> period, 143B).
 * disasm: framed; pushfd; cli;
 *   if ([ebp+0xc] == 0) p = 0xd68d;
 *   else p = (u64)0x2710 * [ebp+0xc] / 0x2e9c;   ; mul;div, EDX carried, no xor
 *   FUN_000397f1([ebp+8], p);
 *   <restore-IF>; leave; ret
 * TWO independent walls: (1) the PUSHFD/CLI + saved-flags IF-restore asm idiom;
 * (2) the true 64-bit `mul; div` with EDX carried live from mul into div
 * (cf. 0x39495/0x39846 -- Watcom 9.5b has no 64-bit integer type and always
 * inserts `xor edx` before an unsigned div, so `f7 ..; f7 ..` is unreachable).
 * Parked. */
