/* frameless @ 0x26c78: move pool-A object `node` to coords (x,y,z).
   Clamp: if tile-x ((short)x>>8) < 1 -> x=0x7e00, >= 0x7f -> x=0x200; tile-y < 1 ->
   y=0x5e00, >= 0x5f -> y=0x200; z &= ~0xf000. Compute the g_grid_heads spatial-grid cells for
   the new and current coords; if the cell changed, unlink from the old cell's doubly-
   linked id list (patch prev->next / cell head, next->prev, clear flag p[0xa]&4) and
   head-insert into the new cell (node->next=head, head->prev=id, cell=id, set flag).
   Finally store the new coords at node+4/6/8. Combines 0x26da8 (unlink) + 0x26e18
   (insert) inline.

   PARKED near-miss 291B vs 292B target (NOT matched), first diff at 0x8. Structure is
   instruction-for-instruction identical incl. all fixup sites; every remaining diff is a
   register-encoding byte from ONE allocation-ranking swap: target ranks [ty=EDX, tx=EBX,
   x=ECX, temps=ESI, y=EDI]; ours converges on [ty=EDX, x=EBX, y=ECX, tx/pold=ESI,
   temps=EDI] (tx-web vs x swapped, y vs temp-cluster swapped). The 1-byte length delta is
   a knock-on: with pold homed byte-addressable (EBX) the target builds its y-part via
   `mov bx,[eax+6]; and bh,0x7f; xor bl,bl; movsx ebx,bx; mov esi,ebx` (28B); ours, with
   pold in ESI, uses `and esi,0x7f00` (27B). Tried: register hints (ignored), decl-order
   flips (inert), OR-operand swap (regressed), tx/ty def split (regressed to 16-bit sar),
   param-copy split x0/y0 (copies not coalesced, +11B), q=pold hoist (inert). Same
   register-role wall family as siblings 0x26da8 / 0x26e18.

   WIN kept in this file: the 0x26da8-family CSE wall (target re-reads *node after the
   `*q = *node` store where -oneatx CSE-merges the loads) IS source-reachable — cast the
   two post-store re-reads through `*(volatile unsigned short *)node` and only those
   loads split (fresh `cmp word [eax],0` + `mov r16,[eax]`), leaving all other codegen
   untouched. */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];

void move_entity_xyz(unsigned char *node, int x, int y, int z)
{
    short ty;
    short tx;
    unsigned short *pnew;
    unsigned short *pold;
    unsigned short t;
    unsigned short *q;
    unsigned short head;

    tx = (short)x >> 8;
    ty = (short)y >> 8;
    if (tx < 1) x = 0x7e00;
    if (ty < 1) y = 0x5e00;
    if (tx >= 0x7f) x = 0x200;
    if (ty >= 0x5f) y = 0x200;
    z &= ~0xf000;

    pnew = g_grid_heads + (((short)(y & 0x7f00) >> 1) | (((short)x >> 8) & 0x7f));
    pold = g_grid_heads + (((short)(*(unsigned short *)(node + 6) & 0x7f00) >> 1) | (((int)*(short *)(node + 4) >> 8) & 0x7f));
    if (pnew != pold) {
        if (node[0xa] & 4) {
            q = pold;
            t = *(unsigned short *)(node + 2);
            if (t != 0)
                q = (unsigned short *)(g_entity_pool + t);
            *q = *(unsigned short *)node;
            if (*(volatile unsigned short *)node != 0) {
                q = (unsigned short *)(g_entity_pool + *(volatile unsigned short *)node);
                *(unsigned short *)((unsigned char *)q + 2) = *(unsigned short *)(node + 2);
            }
            node[0xa] &= 0xfb;
        }
        if (!(node[0xa] & 4)) {
            t = (unsigned short)(node - g_entity_pool);
            *(unsigned short *)(node + 2) = 0;
            head = *pnew;
            *(unsigned short *)node = head;
            if (head != 0) {
                q = (unsigned short *)(g_entity_pool + head);
                *(unsigned short *)((unsigned char *)q + 2) = t;
            }
            *pnew = t;
            node[0xa] |= 4;
        }
    }
    *(unsigned short *)(node + 4) = x;
    *(unsigned short *)(node + 6) = y;
    *(unsigned short *)(node + 8) = z;
}
