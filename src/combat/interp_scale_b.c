/* frameless @ 0x2d808: interpolate/scale. base = obj[0x4c]; delta = obj[0x4d] - base (short).
   If delta==0 return 0. Else scale param_2 by delta and signed-divide: prod/base when delta<0,
   prod/(0xff-base) when delta>0. prod = (unsigned short)param_2 * delta. Branch order: neg
   computes product then divisor, pos computes divisor first. Sibling of 0x2d868 (0x48/0x49)
   and 0x2d7a8 (0x50/0x51). Return int (IDIV quotient in EAX).

   DIST 51->35. Same two levers as the trio: (1) param_2 `unsigned short` -> eager ECX load +
   `xor edx,edx; mov dx,cx` per-branch mask; (2) `unsigned char base = obj[0x4c]` local raises
   obj to volatile EAX; first diff 0x2->0xb. Residual (35B) is the shared register-role/schedule
   tie: target keeps delta in DX (movsx per branch, reloads obj[0x4c] in neg to keep obj alive),
   Watcom here caches base in EBX and stashes delta in EAX (mov eax,edx; cwde). Not source-
   reachable; CSE re-unifies obj[0x4c] with base so the reload can't be forced. */
int interp_scale_b(unsigned char *obj, unsigned short param_2)
{
    unsigned char base = obj[0x4c];
    short delta = obj[0x4d] - base;
    int prod;
    int divisor;
    if (delta == 0)
        return 0;
    if (delta < 0) {
        prod = param_2 * delta;
        divisor = base;
    } else {
        divisor = 0xff - base;
        prod = param_2 * delta;
    }
    return prod / divisor;
}
