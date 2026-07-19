/* frameless @ 0x2d7a8: interpolate/scale. base = obj[0x50]; delta = obj[0x51] - base (short).
   If delta==0 return 0. Else scale param_2 by delta and signed-divide: prod/base when delta<0,
   prod/(0xff-base) when delta>0. prod = (unsigned short)param_2 * delta (16-bit param, signed
   delta, 32-bit imul -> idiv). Third of the R/G/B ramp trio (0x2d808=0x4c/0x4d,
   0x2d868=0x48/0x49). Return type int (IDIV quotient in EAX).

   DIST 50->34. Two source levers found: (1) param_2 is `unsigned short` not `unsigned int`
   -- that is what makes the target load it eagerly into ECX at entry and mask per-branch via
   `xor edx,edx; mov dx,cx` (the 16-bit zero-extend idiom), 50->39. (2) an explicit
   `unsigned char base = obj[0x50]` local raises obj's allocation priority so it lands in
   volatile EAX (matching target) instead of callee-saved EBX, 39->34; first diff moves 0x2->0xb.

   Residual (34B) is a register-role/scheduling tie the source cannot reach: the target keeps
   delta live in DX and reads it just-in-time (movsx from dx per branch, reloading obj[0x50] in
   the neg branch to keep obj alive in EAX), whereas Watcom here caches `base` in EBX, frees obj
   early, and stashes delta into EAX up front (mov eax,edx; cwde). Same 3-cycle EAX<->EDX<->EBX
   the trio shares; CSE keeps re-unifying obj[0x50] with base, so the reload cannot be forced. */
int interp_scale_a(unsigned char *obj, unsigned short param_2)
{
    unsigned char base = obj[0x50];
    short delta = obj[0x51] - base;
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
