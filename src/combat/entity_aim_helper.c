/* entity_aim_helper @ 0x2f608 - entity aim/orient helper.
 * If entity link id (+0x44) is nonzero and target node health (+0x14) >= 0,
 * compute dx = x - self.x(+4), dy = y - self.y(+6); store facing byte at +0x1a
 * from vec_to_angle(dx,dy); h = sum_of_squares_call(dx,dy); dz = z - (self.z(+8)+0x80);
 * store +0x1b from vec_to_angle(dz,h); set +0x19=0x2b, OR bit3 into flags +0xa,
 * then call pool_accessor_44(p) and dispatch_jt45(p).
 *
 * NEAR-MISS (reloc-aware bytediff 14, was 22). RELOC-AWARE match: NO. Prologue
 * (through 0x25), the dx/dy movsx+push pair (0x3a-0x41), and the whole suffix
 * (0x42 through the ret) now match exactly under reloc masking.
 *
 * PROVENANCE WIN (this session, AXIS 2): the dx/dy pair used to seat in the
 * swapped ESI<->EDI roles (ours dx=ESI/dy=EDI vs target dx=EDI/dy=ESI), which
 * the old header called an unreachable "register-role tie floor". TWO
 * behaviour-preserving restructurings flip it to the target's roles:
 *   1. Compute dx BEFORE dy (statement + self-load order), so the x param loads
 *      first and dx is subtracted in-place in EDI (target: mov edi,[x]; sub edi,esi).
 *   2. Swap the args of sum_of_squares_call to (dy, dx). That function is
 *      isqrt32(a*a + b*b) (@0x14c58) -- fully symmetric, so the swap is inert to
 *      behaviour but changes dx/dy live ranges enough to seat dx in EDI.
 * With both, dx=EDI and dy=ESI exactly as the target; bytes 0x3a-0x41 (the two
 * movsx + pushes) become identical.
 *
 * REMAINING GAP (~14 bytes, all in 0x2a-0x39): no longer a register-role swap
 * but a pure instruction-SCHEDULING order. The target loads both params up front
 * (mov edi,[x]; mov eax,[y]) then both self fields (self.y->EDX, self.x->ESI) then
 * both subtracts; ours interleaves (load x, load selfs, sub dx, load y, sub dy).
 * The scheduler's grouping decision is invariant to CPU level (-3s/-4s/-5s), the
 * 'r' reorder toggle, every length-preserving -o bundle, and -zp1/2/4/8 (full
 * flag sweep this session -- all give 14), and to self-load order and param
 * preloading (tested). A scheduler tie floor. Recipe: -4s -oneatx -zp8 -s -zq.
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

    sx = *(short *)(p + 4);
    sy = *(short *)(p + 6);
    dx = (short)(x - sx);
    dy = (short)(y - sy);
    p[0x1a] = vec_to_angle(dx, dy);
    h = sum_of_squares_call(dy, dx); /* isqrt32(a*a+b*b): symmetric, swap seats dx in EDI */
    p[0x1b] = vec_to_angle((short)(z - (*(short *)(p + 8) + 0x80)), h);
    p[0x19] = 0x2b;
    p[0xa] |= 8;
    pool_accessor_44(p);
    dispatch_jt45(p);
}
