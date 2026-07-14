/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Thin wrapper @ 0x3ab69 that forwards
   its 3rd stack arg ([EBP+0xc], into EAX) plus two fixed global buffer addresses (0xbfc4
   in EAX -- immediately overwritten, a dead load kept for byte-fidelity -- and 0xbfcc in
   EBX) and the DS selector (zero-extended into EDX) to the core routine 0x3c644, then
   clears the byte flag at 0x11e48. Framed (PUSH EBX; PUSH EBP; MOV EBP,ESP). The two buffer
   addresses and the flag address are absolute immediates emitted as literal db bytes (they
   equal the resolved linear.bin addresses, no fixup on our side). The CALL 0x3c644 rel32 is
   a real extern call; match_reloc masks the reloc. Body db-transcribed minus the trailing
   RET (frameless void wrapper supplies it); all regs in the wrapper modify set. */
extern void FUN_0003c644(void);
extern void __ab69body(void);
#pragma aux __ab69body = "db 83" "db 85" "db 137" "db 229" "db 184" "db 196" "db 191" "db 0" "db 0" "db 140" "db 218" "db 187" "db 204" "db 191" "db 0" "db 0" "db 139" "db 69" "db 12" "db 15" "db 183" "db 210" "call FUN_0003c644" "db 198" "db 5" "db 72" "db 30" "db 1" "db 0" "db 0" "db 93" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003ab69 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003ab69(void)
{
    __ab69body();
}
