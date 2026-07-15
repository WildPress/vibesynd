/* frameless @ 0x2d808: interpolate/scale. delta = obj[0x4d] - obj[0x4c] (as short). If
   0 return 0. Otherwise scale (param_2 & 0xffff) by delta and divide (signed) by obj[0x4c]
   when delta < 0, or by 0xff - obj[0x4c] when delta > 0. Sibling of 0x2d868 (offsets
   0x48/0x49). Branch order: neg computes product then divisor, pos computes divisor first.

   PARKED near-miss (NOT matched; logic correct). Register/load-scheduling wall: the
   target loads BOTH params up front (obj->EAX, param_2->ECX, masks via `mov dx,cx`
   per branch) and keeps EBX as byte scratch; every C spelling we tried puts obj in EBX
   and defers the param_2 load, and the divergence cascades (cpermute 52/84). Its sibling
   0x2d868 (offsets 0x48/0x49) walls identically -- not attempted. */
int interp_scale_b(unsigned char *obj, unsigned int param_2)
{
    short delta = obj[0x4d] - obj[0x4c];
    int prod;
    int divisor;
    if (delta == 0)
        return 0;
    if (delta < 0) {
        prod = (param_2 & 0xffff) * delta;
        divisor = obj[0x4c];
    } else {
        divisor = 0xff - obj[0x4c];
        prod = (param_2 & 0xffff) * delta;
    }
    return prod / divisor;
}
