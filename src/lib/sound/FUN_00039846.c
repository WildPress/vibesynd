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
extern void __b00039846(void);
#pragma aux __b00039846 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 186" "db 0" "db 0" "db 0" "db 0" "db 184" "db 64" "db 66" "db 15" "db 0" "db 139" "db 93" "db 12" "db 247" "db 243" "db 80" "db 255" "db 117" "db 8" "db 232" "db 140" "db 255" "db 255" "db 255" "db 131" "db 196" "db 8" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00039846 modify [eax ebx ecx edx esi edi ebp];
void FUN_00039846(void) { __b00039846(); }
