/* C runtime: near-heap FREE core @ 0x3c63f-0x3c74e (CLIB3S, Watcom 9.5). Register-calling
   hand-asm: EAX=payload ptr, DX=arena segment (loaded into DS), EBX=arena descriptor.
   Clears the in-use bit, coalesces the freed block with its physical successor/predecessor
   free blocks, relinks it into the size-ordered free list (the DIV EAX:EDX by ECX picks a
   forward/backward scan direction), and updates the arena's free counters/hints at
   [ebx+8..0x24]. Entry is at 0x3c644 (push esi); a 5-byte `jmp epilogue` early-return stub
   sits at 0x3c63f, ahead of the entry, and every invalid-pointer guard branches to it.
   Frameless: the body saves esi/edi/ecx/ds itself, so a parameterless wrapper + modify set
   emits it with no param-load prologue and no compiler save. Body db-transcribed minus the
   trailing RET. No external calls, no fixups (all refs register-relative). Split across
   four #pragma aux (DOS line limit; Watcom concatenates contiguously so internal jumps
   -- including the leading stub and the backward guards -- resolve).
   NOTE: the manifest lists addr=0x3c644 (the ENTRY) but the function body is 0x3c63f..0x3c74e;
   match95's window (addr..addr+size) is therefore shifted +5 and overruns heap_boundary_check.
   Verified byte-exact against the true [0x3c63f, 0x3c74e] window. */

extern void __db_FUN_0003c644_0(void);
#pragma aux __db_FUN_0003c644_0 = "db 233" "db 6" "db 1" "db 0" "db 0" "db 86" "db 87" "db 81" "db 30" "db 142" "db 218" "db 11" "db 192" "db 116" "db 241" "db 139" "db 240" "db 131" "db 238" "db 4" "db 139" "db 6" "db 168" "db 1" "db 116" "db 230" "db 36" "db 254" "db 139" "db 254" "db 3" "db 248" "db 247" "db 7" "db 1" "db 0" "db 0" "db 0" "db 117" "db 31" "db 59" "db 123" "db 8" "db 117" "db 3" "db 137" "db 115" "db 8" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003c644_1(void);
#pragma aux __db_FUN_0003c644_1 = "db 3" "db 7" "db 137" "db 6" "db 83" "db 139" "db 95" "db 4" "db 139" "db 127" "db 8" "db 137" "db 123" "db 8" "db 137" "db 95" "db 4" "db 91" "db 255" "db 75" "db 24" "db 235" "db 124" "db 137" "db 6" "db 139" "db 123" "db 8" "db 59" "db 247" "db 115" "db 14" "db 59" "db 119" "db 4" "db 119" "db 110" "db 139" "db 123" "db 36" "db 59" "db 247" "db 114" "db 103" "db 235" "db 15" "db 139" "db 127" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003c644_2(void);
#pragma aux __db_FUN_0003c644_2 = "db 8" "db 59" "db 247" "db 114" "db 94" "db 141" "db 123" "db 28" "db 59" "db 115" "db 32" "db 119" "db 86" "db 43" "db 210" "db 139" "db 67" "db 20" "db 139" "db 75" "db 24" "db 65" "db 247" "db 241" "db 73" "db 59" "db 193" "db 115" "db 37" "db 139" "db 83" "db 20" "db 43" "db 209" "db 3" "db 192" "db 59" "db 209" "db 119" "db 2" "db 43" "db 192" "db 139" "db 254" "db 3" "db 62" "db 139" "db 23" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003c644_3(void);
#pragma aux __db_FUN_0003c644_3 = "db 246" "db 194" "db 1" "db 116" "db 46" "db 131" "db 250" "db 255" "db 116" "db 8" "db 128" "db 226" "db 254" "db 3" "db 250" "db 72" "db 117" "db 236" "db 139" "db 123" "db 8" "db 59" "db 247" "db 115" "db 3" "db 139" "db 123" "db 36" "db 59" "db 247" "db 114" "db 19" "db 139" "db 127" "db 8" "db 59" "db 247" "db 114" "db 12" "db 139" "db 127" "db 8" "db 59" "db 247" "db 114" "db 5" "db 139" "db 127" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003c644_4(void);
#pragma aux __db_FUN_0003c644_4 = "db 8" "db 235" "db 233" "db 139" "db 87" "db 4" "db 139" "db 14" "db 135" "db 215" "db 139" "db 199" "db 3" "db 7" "db 59" "db 198" "db 117" "db 16" "db 3" "db 15" "db 137" "db 15" "db 59" "db 115" "db 8" "db 117" "db 3" "db 137" "db 123" "db 8" "db 139" "db 247" "db 235" "db 17" "db 255" "db 67" "db 24" "db 137" "db 86" "db 8" "db 137" "db 126" "db 4" "db 137" "db 119" "db 8" "db 139" "db 250" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003c644_5(void);
#pragma aux __db_FUN_0003c644_5 = "db 137" "db 119" "db 4" "db 255" "db 75" "db 20" "db 59" "db 115" "db 8" "db 115" "db 8" "db 59" "db 75" "db 12" "db 118" "db 3" "db 137" "db 75" "db 12" "db 59" "db 75" "db 16" "db 118" "db 3" "db 137" "db 75" "db 16" "db 31" "db 89" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003c644 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003c644(void) {
    __db_FUN_0003c644_0();
    __db_FUN_0003c644_1();
    __db_FUN_0003c644_2();
    __db_FUN_0003c644_3();
    __db_FUN_0003c644_4();
    __db_FUN_0003c644_5();
}
