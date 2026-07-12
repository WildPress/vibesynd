/* framed @ 0x39495 (49B): scale a value into FUN_00039467.
   Disasm (authoritative):
     push ebp; mov ebp,esp (8b ec); push esi; push edi
     mov eax,0                       ; r = 0
     cmp dword [ebp+8],0xd68d ; jnc .call
     mov eax,[ebp+8]
     mov ebx,0x20bc                  ; 8380
     mov ecx,0x2710                  ; 10000
     mul ecx                         ; edx:eax = x * 10000 (UNSIGNED, 64-bit)
     div ebx                         ; eax = (edx:eax) / 8380  (64/32 divide)
   .call: push eax; call 0x39467; add esp,4; pop edi; pop esi; leave; ret
   => stack calling (params at [ebp+8]); returns FUN_00039467(r).
   Meaning: r = (x < 0xd68d) ? (unsigned)((u64)x * 10000 / 8380) : 0.

   WALL (§3 / not source-reachable with Watcom 9.5b). Three independent blockers,
   the first fatal:
   1. The target computes a TRUE 64-bit intermediate: `mul ecx` sets EDX:EAX to the
      full product and `div ebx` consumes EDX:EAX with NO intervening `xor edx`.
      That is a 64-bit muldiv. Watcom 9.5b in this container has NO 64-bit integer
      type: `unsigned long long` -> E1060 "Invalid type", `__int64` -> E1009 (not a
      keyword). With plain `unsigned int`, Watcom NEVER emits `mov ecx,imm; mul ecx`
      for a constant multiplier -- it either `imul eax,[..],0x2710` (-od/-os/-ot) or
      strength-reduces to LEA/SHL (-oneatx), and always inserts `xor edx,edx` before
      the unsigned `div`. So `f7 e1 f7 f3` (mul;div, no xor) is unreachable.
   2. Frame encoding: the target's `mov ebp,esp` is `8b ec`; EVERY recipe here emits
      `89 e5` (first diff lands at 0x1 for all framed variants).
   3. `mov eax,0` (b8 00000000) vs our `xor eax,eax` (31 c0).
   The logic below is correct and kept as a reference; the codegen mismatch is a
   compiler-capability wall, not a lever/permuter problem.
   Closest framed recipe (small, correct logic): -3s -of -os -s -zq. */
extern int FUN_00039467(unsigned int);

unsigned int FUN_00039495(unsigned int x)
{
    unsigned int r = 0;
    if (x < 0xd68d)
        r = x * 0x2710 / 0x20bc;
    return FUN_00039467(r);
}
