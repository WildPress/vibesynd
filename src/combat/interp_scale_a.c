/* frameless @ 0x2d7a8: interpolate/scale. delta = obj[0x51] - obj[0x50] (as short). If 0
   return 0. Else scale (param_2 & 0xffff) by delta, signed-divide by obj[0x50] when
   delta<0 or by 0xff-obj[0x50] when delta>0. Third of the R/G/B ramp trio
   (0x2d808=0x4c/0x4d, 0x2d868=0x48/0x49). Return type int (IDIV, result in EAX).

   PARKED WALL (§3 register-role tie-break; NOT matched, logic byte-correct except reg roles).
   Target holds obj in volatile EAX and param_2 in ECX (masked per-branch via `mov dx,cx`);
   every C spelling puts obj in callee-saved EBX -> first diff at 0x2 (8b44 vs 8b5c), cascades.
   Best near-miss 84/85B with `(param_2 & 0xffff)`. `(unsigned short)param_2` loads param
   early but still into EDX not ECX and grows to 94B. Identical wall to matched-shape sibling
   0x2d808 (parked cpermute 52/84). Not source-reachable; allocator tie-break. */
int interp_scale_a(unsigned char *obj, unsigned int param_2)
{
    short delta = obj[0x51] - obj[0x50];
    int prod;
    int divisor;
    if (delta == 0)
        return 0;
    if (delta < 0) {
        prod = (param_2 & 0xffff) * delta;
        divisor = obj[0x50];
    } else {
        divisor = 0xff - obj[0x50];
        prod = (param_2 & 0xffff) * delta;
    }
    return prod / divisor;
}
