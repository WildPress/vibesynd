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

   NEAR-MISS, size now exact-length-adjacent (197 target). EDIT-DIST 28 (was 74).
   Three fixes closed most of the gap: (1) hoisting `short sel = param_2` to function
   scope so param_2 is a single register live across BOTH branches -- this makes the
   allocator reserve the 3rd callee-saved EDI (`push edi`) and route the slot*10000
   multiply chain and rem through it, matching the target's obj=EBX / param_2=ESI /
   chain=EDI roles. (2) `short sel` (not int) keeps param_2 raw in ESI (movsx at use)
   instead of an eager sign-extend. (3) writing the overrun scale as
   (1000 - labs(rem)) * param_2 makes the imul land param_2 as the destination,
   reproducing the target's `mov eax,edx; movsx edx,si; imul edx,eax` and ecx divisor.
   Residual (~28): two coupled Watcom ties. The flag test is emitted as
   `mov dx,[obj+0x1c]; test dx` rather than the target's `test WORD[obj+0x1c]` (the
   eager param_2 load reorders the two entry loads), and the slot*10000 LEA chain
   accumulates in EAX before the final `lea edi,[eax+0x3e8]` rather than in EDI in
   place. Priming EDX via the split flag-load is what gives the matching
   `xor edx,edx; mov dx,ax` field extraction, so a memory-form flag test regresses the
   field-extract idiom instead (confirmed by the param_2-reassign variant). Codegen
   tie floor. */
extern int chain_sum_3a(unsigned char *obj);
extern int labs(int v);
extern int interp_scale_c(unsigned char *obj, unsigned int v);

int anim_speed_select(unsigned char *obj, short param_2)
{
    int r;
    short sel = param_2;

    if (*(unsigned short *)(obj + 0x1c) & 0x1002) {
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

    r = interp_scale_c(obj, (unsigned short)sel);
    return (short)r < 0xc ? 0xc : r;
}
