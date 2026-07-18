/* entity_aim_helper @ 0x2f608 - entity aim/orient helper.
 * If entity link id (+0x44) is nonzero and target node health (+0x14) >= 0,
 * compute dx = x - self.x(+4), dy = y - self.y(+6); store facing byte at +0x1a
 * from FUN_0004d221(dx,dy); h = sum_of_squares_call(dx,dy); dz = z - (self.z(+8)+0x80);
 * store +0x1b from FUN_0004d221(dz,h); set +0x19=0x2b, OR bit3 into flags +0xa,
 * then call pool_accessor_44(p) and dispatch_jt45(p).
 *
 * NEAR-MISS (PARKED, register-role tie-break wall, §3). Logic byte-identical:
 * 9-byte prologue prefix + 80-byte suffix (from the +0x1a store through the ret)
 * match exactly under reloc masking. First diff at 0x9: the link id lands in
 * EAX (ours) vs EDX (target) at `mov si,ax`/`mov si,dx`. That EDX choice frees
 * EDX for the p+6 field (target `mov dx,[ebx+6]; sub eax,edx` reg-reg) where our
 * EAX-id gives `sub ax,[ebx+6]` reg-mem, and swaps the dx/dy cache regs ESI<->EDI
 * (so the arg pushes read 56 57 vs 57 56); this also makes ours 154B vs 156B.
 * The g_entity_pool+id materialization, both guards, all globals, all field offsets and
 * arithmetic are correct. Tried: node materialization, stmt/decl reorder,
 * nested-if vs early-return, inline-vs-named-temp, recipes -oneatx/-or/-ot/-oe/
 * -os/-ox/-oh/-oa/-ob/-oi/-od/-op and -4s/-5s/-6s -- all give byte-identical
 * output; id->EAX is deterministic and not source-reachable.
 */

extern unsigned char g_entity_pool[];
extern unsigned char FUN_0004d221(int a, int b);
extern short sum_of_squares_call(int a, int b);
extern void pool_accessor_44(unsigned char *p);
extern void dispatch_jt45(unsigned char *p);

void entity_aim_helper(unsigned char *p, int x, int y, int z)
{
    unsigned short id;
    unsigned char *node;
    int dx, dy;
    short h;

    id = *(unsigned short *)(p + 0x44);
    if (id == 0)
        return;
    node = g_entity_pool + id;
    if (*(short *)(node + 0x14) < 0)
        return;

    dy = (short)(y - *(short *)(p + 6));
    dx = (short)(x - *(short *)(p + 4));
    p[0x1a] = FUN_0004d221(dx, dy);
    h = sum_of_squares_call(dx, dy);
    p[0x1b] = FUN_0004d221((short)(z - (*(short *)(p + 8) + 0x80)), h);
    p[0x19] = 0x2b;
    p[0xa] |= 8;
    pool_accessor_44(p);
    dispatch_jt45(p);
}
