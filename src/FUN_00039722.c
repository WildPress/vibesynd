/* @ 0x39722 (37B): interrupt-critical-section teardown.
   Disasm (authoritative):
     push esi; push edi
     pushfd; cli                     ; save EFLAGS, disable interrupts
     mov esi,0xf
   .loop: push esi; call 0x396d5; add esp,4; dec esi; jge .loop   ; f(15..0)
     push ebp; mov ebp,esp
     test byte [ebp+5],2             ; test saved-EFLAGS IF bit (bit 9)
     cli
     jz .skip; sti                   ; re-enable IF iff it was set
   .skip: pop ebp
     popfd; pop edi; pop esi; ret

   SKIP-ASM-IDIOM. The ordinary part (a countdown loop calling FUN_000396d5(i)
   for i = 15..0) is trivial C, but the surrounding construct is pure x86 flag/
   interrupt machinery that headerless C cannot emit:
     - PUSHFD/POPFD to save & restore the processor flags,
     - CLI/STI to gate interrupts,
     - a nested `push ebp; mov ebp,esp; test byte[ebp+5],2; sti` that reads the
       *saved* EFLAGS off the stack to conditionally restore the interrupt-enable
       flag (an inlined "restore interrupt state" helper).
   Watcom has no include-free way to produce PUSHFD/POPFD/CLI/STI (the _disable/
   _enable intrinsics need <dos.h>/<conio.h> and would not reproduce the PUSHFD/
   POPFD save-restore framing anyway). Identical idiom appears inline at the tail
   of the parent FUN_000395b6. Not writable from C; parked. */
