/* prefix @ 0xfa88 -- Z-probe (twin of 0xfa18, callee tile_passability_test_b). Try the passability test at
 * z+0x7f, z-1, z-0x81; return first nonzero, else a floor-masked z. Frameless, stack-calling.
 * Recipe: -4s -oneatx -zp8 -s -zq. Near-miss 112/112. First diff at 0x1: prologue seats y in EBP
 * (push ebx/edi/ebp) where target uses ESI (push ebx/esi/edi) -- a register-choice tie invariant to
 * the source x flag cross-product (2026-07-20). The b+1 lea-vs-mov+inc is a second downstream tie. */
extern unsigned tile_passability_test_b(int x, int y, int z);

unsigned z_probe_b(short x, short y, int z)
{
    int b = z + 0x7f;
    unsigned r = tile_passability_test_b(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    r = tile_passability_test_b(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    if ((short)b < 0)
        return (unsigned)(b + 1) & 0xffffff80;
    r = tile_passability_test_b(x, y, (short)b);
    if ((short)r != 0)
        return r;
    return (unsigned)b & 0xffffff80;
}
