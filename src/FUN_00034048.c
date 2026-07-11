/* frameless @ 0x34048: turn/step a direction value `cur` toward target `tgt`.
   Delta d = (low byte of cur) - tgt as a signed 16-bit angle; snap (cur - d) when
   within +/-0x20, else step by 0x20. Watcom forms the byte-masked operand in place
   (mov edx,eax; xor dh,ah), keeps cur live in eax, and holds d as a full int so
   cur - d reuses edx and all three results share ebx = cur - d.

   PARKED near-miss (49/56, NOT matched -> not in recipes/manifest). The whole
   structure matches: eager cur-d hoist, byte-mask-in-place, branch layout, all
   correct. The one divergence is a register-ROLE tie-break: the target keeps cur
   in EAX and d in EDX; Watcom insists on loading cur into EDX here, which flips
   the downstream encodings (cwde<->movsx, lea<->add, cmp ax<->cmp dx). Confirmed
   a wall: int/short params, mask-in-place vs separate stmt, and 8000 cpermute
   variants all converge to this same swapped-role form. Same class as 0x26e18. */
int FUN_00034048(int cur, int tgt)
{
    int m = cur & 0xffff00ff;
    int d = m - tgt;
    if ((short)d < 0) {
        if ((short)d < -0x20)
            return cur + 0x20;
        return cur - d;
    }
    if ((short)d > 0) {
        if ((short)d > 0x20)
            return cur - 0x20;
        return cur - d;
    }
    return cur;
}
