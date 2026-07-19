/* entity_aim_helper @ 0x2f608 - entity aim/orient helper.
 * If entity link id (+0x44) is nonzero and target node health (+0x14) >= 0,
 * compute dx = x - self.x(+4), dy = y - self.y(+6); store facing byte at +0x1a
 * from vec_to_angle(dx,dy); h = sum_of_squares_call(dx,dy); dz = z - (self.z(+8)+0x80);
 * store +0x1b from vec_to_angle(dz,h); set +0x19=0x2b, OR bit3 into flags +0xa,
 * then call pool_accessor_44(p) and dispatch_jt45(p).
 *
 * NEAR-MISS (dist 35, was 42). RELOC-AWARE match: NO, but the entire prologue
 * (through 0x25) and the entire suffix (0x42 through the ret) now match exactly
 * under reloc masking. Two source fixes bought it:
 *   1. Reading the +0x44 link id at the point of use (the guard AND the pool index,
 *      i.e. `if (id != 0) { node = &pool[id]; ... }`) instead of caching it in a
 *      named local. The compiler CSEs the two reads to a single load, and that load
 *      now lands in DX (matching the target) instead of AX. This fixed the whole
 *      head-register cascade the old header called "id->EAX deterministic".
 *   2. Materializing self.x/self.y as short locals, which gives the target's
 *      reg-reg `mov dx,[ebx+6]; sub eax,edx` shape (was reg-mem `sub ax,[ebx+6]`)
 *      and restores the 156-byte length.
 *
 * REMAINING GAP (~11 real bytes, all in 0x26-0x41): the dx/dy pair lands in the
 * swapped ESI<->EDI pair (target dx=EDI, dy=ESI; ours dx=ESI, dy=EDI), so the two
 * arg pushes read 57 56 vs 56 57 at both call sites. This swap is a stable
 * Watcom 9.5 allocator decision -- invariant to statement order (dy-first vs
 * dx-first), declaration order (dx,dy vs dy,dx), self-load order, and param
 * preloading (all tested). Not source-reachable; a register-role tie floor.
 */

extern unsigned char g_entity_pool[];
extern unsigned char vec_to_angle(int a, int b);
extern short sum_of_squares_call(int a, int b);
extern void pool_accessor_44(unsigned char *p);
extern void dispatch_jt45(unsigned char *p);

void entity_aim_helper(unsigned char *p, int x, int y, int z)
{
    unsigned char *node;
    int dx, dy;
    short h;
    short sx, sy;

    if (*(unsigned short *)(p + 0x44) == 0)
        return;
    node = g_entity_pool + *(unsigned short *)(p + 0x44);
    if (*(short *)(node + 0x14) < 0)
        return;

    sy = *(short *)(p + 6);
    sx = *(short *)(p + 4);
    dy = (short)(y - sy);
    dx = (short)(x - sx);
    p[0x1a] = vec_to_angle(dx, dy);
    h = sum_of_squares_call(dx, dy);
    p[0x1b] = vec_to_angle((short)(z - (*(short *)(p + 8) + 0x80)), h);
    p[0x19] = 0x2b;
    p[0xa] |= 8;
    pool_accessor_44(p);
    dispatch_jt45(p);
}
