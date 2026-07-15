/* frameless @ 0x2d8c8: per-object anim frame-duration / speed selector.
   Called from FUN_0002d6c8 / projectile_step as obj[0x54] = anim_speed_select(obj, obj[0x55]).

   If ((u16)obj[0x1c] & 0x1002) is set: pick a duration slot from the 2-bit field
   (obj[0x3c] & 0x18) >> 3, form base = slot*10000 + 1000, and subtract the
   pool-chain elapsed sum FUN_000376f8(obj). When that remainder goes negative
   (elapsed overran the base) scale param_2 down by the overrun fraction
   (param_2 * (1000 - labs(rem)) / 1000, stride 0x3e8 = 1000) and floor to 0x10.
   Feed the (u16) value to the ramp sibling interp_scale_c and floor its result to
   0x10. When the flag bit is clear, run the ramp on raw param_2 and floor to 0xc.
   labs = FUN_0003aed8; pool-chain sum = FUN_000376f8. Same idiv-by-1000 /
   labs shape as the 0x2d7a8 / 0x2d808 / 0x2d868 sibling trio (this is their
   dispatcher). Stack calling convention (-4s): params read from [ESP+0x10/0x14].

   NEAR-MISS (195/197B, NOT matched). Field 0x3c is read as a WORD in the target
   (mov ax,word[obj+0x3c]) -- the source now matches that. The residual wall is a
   register-ROLE tie: the target routes the slot*10000 multiply chain through EDI (a
   3rd callee-saved reg it pushes) and holds both params in EBX/ESI up front, while
   our Watcom keeps the chain in EAX and pushes only EBX/ESI. First diff is the entry
   `push edi` (byte 0x2); no C form makes the allocator reserve EDI here. Wall. */
extern int FUN_000376f8(unsigned char *obj);
extern int FUN_0003aed8(int v);
extern int interp_scale_c(unsigned char *obj, unsigned int v);

int anim_speed_select(unsigned char *obj, short param_2)
{
    int r;

    if (*(unsigned short *)(obj + 0x1c) & 0x1002) {
        int base = ((*(unsigned short *)(obj + 0x3c) & 0x18) >> 3) * 10000 + 1000;
        int rem = base - FUN_000376f8(obj);
        int sel = param_2;
        if (rem < 0) {
            sel = param_2 * (1000 - FUN_0003aed8(rem)) / 1000;
            if ((short)sel < 0x10)
                sel = 0x10;
        }
        r = interp_scale_c(obj, (unsigned short)sel);
        return (short)r < 0x10 ? 0x10 : r;
    }

    r = interp_scale_c(obj, (unsigned short)param_2);
    return (short)r < 0xc ? 0xc : r;
}
