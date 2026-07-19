/* frameless @ 0x2d868: interpolate/scale. base = obj[0x48]; delta = obj[0x49] - base (short).
   If delta==0 return 0. Else scale param_2 by delta and signed-divide: prod/base when delta<0,
   prod/(0xff-base) when delta>0. prod = (unsigned short)param_2 * delta. First of the R/G/B
   ramp trio (0x2d808=0x4c/0x4d, 0x2d7a8=0x50/0x51). Return int (IDIV quotient in EAX).

   DIST 50->34. Same two levers as the trio: (1) param_2 `unsigned short` -> eager ECX load +
   `xor edx,edx; mov dx,cx` per-branch mask; (2) `unsigned char base = obj[0x48]` local raises
   obj to volatile EAX; first diff 0x2->0xb. Residual (34B) is the shared register-role/schedule
   tie: target keeps delta in DX (movsx per branch, reloads obj[0x48] in neg to keep obj alive),
   Watcom here caches base in EBX and stashes delta in EAX (mov eax,edx; cwde). Not source-
   reachable; CSE re-unifies obj[0x48] with base so the reload can't be forced. */
int interp_scale_c(unsigned char *obj, unsigned short param_2)
{
    unsigned char base = obj[0x48];
    short delta = obj[0x49] - base;
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
