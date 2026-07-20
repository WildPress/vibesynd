/* prefix @ 0xfa18 -- previously-undecoded render-path prefix fn (obj1_full). Z-probe: try the
 * passability test tile_passability_test(x,y,z) at z+0x7f, then z-1, then z-0x81; return the first nonzero
 * result, else a floor-masked z. b tracks z in a full int (EBX); each call sign-extends its low
 * short. Frameless push-ebx/esi/edi, stack-calling. Recipe: -4s -oneatx -zp8 -s -zq.
 *
 * PARKED near-miss, length-EXACT 112/112. NOTE (2026-07-20, corrected): the FIRST diff is at 0x1,
 * not the lea. The prologue caches {b,y,x} in {EBX,ESI,EDI} in the target (push ebx/esi/edi = 53 56 57)
 * but {EBX,EDI,EBP} in ours (53 57 55) -- it skips ESI and seats y in EBP. That is a pure register-choice
 * tie for the 3rd cross-call register, invariant across int-typed locals, y-before-x ordering, and the
 * full 12-flag matrix (source x flag cross-product, 2026-07-20). The lea-vs-mov+inc for `b + 1`
 * (`lea eax,[ebx+1]` vs target `mov eax,ebx; inc eax`) is a SECOND diff downstream; both are genuine
 * 9.5 ties. Twin 0xfa88 (callee tile_passability_test_b) shares the residue. */
extern unsigned tile_passability_test(int x, int y, int z);

unsigned z_probe(short x, short y, int z)
{
    int b = z + 0x7f;
    unsigned r = tile_passability_test(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    r = tile_passability_test(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    if ((short)b < 0)
        return (unsigned)(b + 1) & 0xffffff80;
    r = tile_passability_test(x, y, (short)b);
    if ((short)r != 0)
        return r;
    return (unsigned)b & 0xffffff80;
}
