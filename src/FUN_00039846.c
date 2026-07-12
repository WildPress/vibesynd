/* FUN_00039846 -- SKIP-ASM-IDIOM.
 *
 * Hand-written critical section around a rate-reprogram helper; not reachable
 * from Watcom C (needs PUSHFD/CLI/STI/POPFD interrupt-flag control and an
 * inlined "restore IF from saved EFLAGS" block).
 *
 * disasm (0x39846, 51B):
 *   push ebp; mov ebp,esp; push esi; push edi
 *   pushfd; cli                         ; <-- save+disable interrupts
 *   mov edx,0; mov eax,0xf4240          ; EAX = 1000000
 *   mov ebx,[ebp+0xc]; div ebx          ; EAX = 1000000 / param_2  (rate->period)
 *   push eax; push [ebp+8]; call 0x397f1; add esp,8
 *   push ebp; mov ebp,esp               ; inlined restore-IF helper:
 *   test byte ptr [ebp+5],0x2           ;   was IF set in saved flags?
 *   cli; jz +1; sti                     ;   conditionally re-enable
 *   pop ebp
 *   popfd; pop edi; pop esi; leave; ret ; <-- restore interrupts
 *
 * The CLI/STI/PUSHFD/POPFD sequence and the saved-flags IF-restore are pure
 * asm idioms (cf. playbook: bswap/OUT/IN/LODSD -> SKIP-ASM-IDIOM). No C
 * spelling emits them; parked without a compile attempt.
 */
