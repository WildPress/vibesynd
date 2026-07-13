/* FUN_00039a82 -- SKIP-ASM-IDIOM (voice-stop trampoline, 61B).
 * disasm: mov ebx,esp; mov ebx,[ebx+4]; if ((unsigned)ebx >= 0x10) ret;
 *   ebx <<= 2; edx=0; xchg [ebx+g_bd7a],edx;      ; atomically clear active flag
 *   if (old != 0) { e = [ebx+g_bd3a]; if (e != -1) FUN_000396d5(e); }
 *   mov eax,0x68; jmp FUN_000392ac                ; tail-dispatch key 0x68
 * The `mov ebx,esp` stack read, the `xchg` read-modify-write clear and the
 * computed tail `jmp` are hand asm; Watcom C emits none of them. Parked. */
