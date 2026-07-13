/* FUN_00039994 (cdecl/-4s wrapper). Dispatch through the key-record trampoline
 * FUN_000392ac with key 0x64, forwarding param_1 and the code pointer 0x2c176
 * (a resume label inside FUN_0002bee8) as the trampoline's stack arguments.
 * The trampoline takes its key in EAX and the remaining args on the stack.
 *
 * NEAR-MISS (24/31, -4s -oneatx). The call itself is byte-correct: the
 * `parm [eax]` pragma reproduces `mov eax,0x64` + the two right-to-left stack
 * pushes + `add esp,8` and the masked call rel32. Residual = the region-flavor
 * wall (cf. 0x39188): the target is framed with `push ebp; mov ebp,esp` encoded
 * as `8b ec` (our 9.5b always emits `89 e5`) plus DEAD `push esi/push edi` /
 * `pop edi/pop esi/leave` saves that -oneatx never emits (nothing is live in
 * esi/edi); and it materialises the code pointer 0x2c176 via `mov eax,imm;
 * push eax` where ours emits `push imm32`. The 8b-ec frame alone is fatal (byte
 * 1). Whole-region different-Watcom-flavor wall; parked. */
extern int FUN_000392ac(int key, int a1, int a2);
#pragma aux FUN_000392ac parm [eax];

int FUN_00039994(int param_1)
{
    return FUN_000392ac(0x64, param_1, 0x2c176);
}
