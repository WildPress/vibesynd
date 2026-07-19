/* prefix @ 0xfa18 -- previously-undecoded render-path prefix fn (obj1_full). Z-probe: try the
 * passability test tile_passability_test(x,y,z) at z+0x7f, then z-1, then z-0x81; return the first nonzero
 * result, else a floor-masked z. b tracks z in a full int (EBX); each call sign-extends its low
 * short. Frameless push-ebx/esi/edi, stack-calling. Recipe: -4s -oneatx -zp8 -s -zq.
 *
 * PARKED near-miss, length-EXACT 112/112, 96.8% (regdiff), ONE instruction off at #27: the
 * `b + 1` in the sVar<0 fallback -- target emits `mov eax,ebx; inc eax`, Watcom -oneatx emits the
 * fused `lea eax,[ebx+1]`. Tried `unsigned t=b; ++t;` (copy-propagated back to lea). A lea-vs-mov+inc
 * codegen tie, same class as the parked corpus. Twin 0xfa88 (callee tile_passability_test_b) shares the residue. */
extern unsigned tile_passability_test(int x, int y, int z);

unsigned FUN_0000fa18(short x, short y, int z)
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
