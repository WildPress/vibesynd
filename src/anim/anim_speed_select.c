/* frameless @ 0x2d8c8: per-object anim frame-duration / speed selector.
   Called from aim_step / projectile_step as obj[0x54] = anim_speed_select(obj, obj[0x55]).

   If ((u16)obj[0x1c] & 0x1002) is set: pick a duration slot from the 2-bit field
   (obj[0x3c] & 0x18) >> 3, form base = slot*10000 + 1000, and subtract the
   pool-chain elapsed sum chain_sum_3a(obj). When that remainder goes negative
   (elapsed overran the base) scale param_2 down by the overrun fraction
   (param_2 * (1000 - labs(rem)) / 1000, stride 0x3e8 = 1000) and floor to 0x10.
   Feed the (u16) value to the ramp sibling interp_scale_c and floor its result to
   0x10. When the flag bit is clear, run the ramp on raw param_2 and floor to 0xc.
   labs = labs; pool-chain sum = chain_sum_3a. Same idiv-by-1000 /
   labs shape as the 0x2d7a8 / 0x2d808 / 0x2d868 sibling trio (this is their
   dispatcher). Stack calling convention (-4s): params read from [ESP+0x10/0x14].

   NEAR-MISS, 199 bytes vs 197 target (delta +2), reloc-aware edit-dist 31.

   The prior spelling hoisted `short sel = param_2` to function scope. That made
   Watcom pre-load the flag word into DX (`mov dx,[obj+0x1c]; test dx`) and then
   immediately re-zero DX with `xor edx,edx`, three wasted bytes the target never
   spends -- the target tests memory directly (`test WORD[obj+0x1c],0x1002`). Moving
   `sel` back inside the flag-set block removes the eager DX load, so ours now emits
   the memory-form test byte-for-byte and length drops 200 -> 199. The old header
   predicted a memory-form test would regress the `xor edx,edx; and al,0x18; xor ah,ah;
   mov dx,ax` field extraction, but it does not: that idiom stays identical in this
   spelling. Writing base as `slot*10000 + 1000` in one expression keeps the multiply
   chain and rem in EDI (splitting the +1000 pushes rem into EDX and regresses).

   Residual (31): register-scheduling ties, not reconstruction errors. (1) The target
   loads param_2 into ESI before the memory test so it is live across BOTH paths, which
   lets the flag-clear tail use `mov ax,si`; ours loads param_2 late inside the block,
   so the tail reloads it from `[esp+0x14]`. Restoring the early ESI load requires the
   function-scope `sel` that reintroduces the wasteful DX pre-load, so the two are
   anti-correlated. (2) The slot*10000 LEA chain accumulates in EAX then `lea
   edi,[eax+0x3e8]` rather than in EDI in place, and (3) the idiv divisor is ESI not
   ECX. Same allocator-insensitive class as the 0x128b8 wall. */
extern int chain_sum_3a(unsigned char *obj);
extern int labs(int v);
extern int interp_scale_c(unsigned char *obj, unsigned int v);

int anim_speed_select(unsigned char *obj, short param_2)
{
    int r;

    if (*(unsigned short *)(obj + 0x1c) & 0x1002) {
        short sel = param_2;
        int base = ((*(unsigned short *)(obj + 0x3c) & 0x18) >> 3) * 10000 + 1000;
        int rem = base - chain_sum_3a(obj);
        if (rem < 0) {
            sel = (1000 - labs(rem)) * param_2 / 1000;
            if ((short)sel < 0x10)
                sel = 0x10;
        }
        r = interp_scale_c(obj, (unsigned short)sel);
        return (short)r < 0x10 ? 0x10 : r;
    }

    r = interp_scale_c(obj, (unsigned short)param_2);
    return (short)r < 0xc ? 0xc : r;
}
