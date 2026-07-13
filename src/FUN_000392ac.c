/* FUN_000392ac -- SKIP-ASM-IDIOM (key-record dispatch trampoline, 18B).
 * disasm: mov ebx,esp; mov ebx,[ebx+4]; call FUN_00039280; cmp eax,0;
 *         jz .ret; jmp eax; .ret: ret
 * Reads the first stack arg into EBX and the lookup key from EAX, calls the
 * key-record table lookup FUN_00039280, then TAIL-JMPs the returned handler
 * (forwarding the caller's remaining stack args) or returns if none was found.
 * The explicit `mov ebx,esp` stack read and the computed `jmp eax` tail dispatch
 * are hand-written asm; Watcom C emits neither. This is the target of the many
 * already-matched `mov eax,imm; jmp FUN_000392ac` dispatch stubs. Parked. */
