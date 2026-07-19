/* frameless @ 0x26c78: move pool-A object `node` to coords (x,y,z).
   Clamp: if tile-x ((short)x>>8) < 1 -> x=0x7e00, >= 0x7f -> x=0x200; tile-y < 1 ->
   y=0x5e00, >= 0x5f -> y=0x200; z &= ~0xf000. Compute the g_grid_heads spatial-grid cells for
   the new and current coords; if the cell changed, unlink from the old cell's doubly-
   linked id list (patch prev->next / cell head, next->prev, clear flag p[0xa]&4) and
   head-insert into the new cell (node->next=head, head->prev=id, cell=id, set flag).
   Finally store the new coords at node+4/6/8. Combines 0x26da8 (unlink) + 0x26e18
   (insert) inline.

   NEAR-MISS 291B vs 292B target (NOT matched), first diff at 0xc. IMPROVED dist 78 -> 67
   with two order re-seeds (a full knob sweep over {def order} x {pnew,pold operand order}
   picks this exact pair as the minimum):
     - compute `ty` BEFORE `tx`: flips param x out of the saved EBX into scratch ECX so it
       matches the target (was x=EBX/y=ECX; now x=ECX matches target). Knock-on: y takes
       saved EBX (target wants EDI) and tx/ty land EDX/ESI (target EBX/EDX).
     - `pold` OR-operands swapped (x-tile part first): realigns the unlink/insert temp
       register lattice, dropping 75 -> 67 with no length or first-diff change.

   REMAINING GAP: the same register-role rotation the sibling 0x26da8 / 0x26e18 walls show.
   Everything is instruction-for-instruction identical incl. all fixup sites; the residual
   is one allocation choice: target reserves the byte-addressable saved EBX for the
   pnew/pold y-part temp (freed from tx), so its pold y-part is `mov bx,[n+6]; and bh,0x7f;
   xor bl,bl; movsx ebx,bx` (byte ops, 292B); ours homes that temp in ESI (y grabbed EBX),
   so it uses `mov si,[n+6]; and esi,0x7f00; movsx` (word op, 291B -- 1B shorter). The first
   param referenced always claims EBX ahead of the later temp, so no source order frees EBX
   for the temp without pushing a param back into it. Tried and rejected: clamp reorder
   (regressed), decl-order flip (inert), cmp-operand swap (inert), pold hoist above clamps
   (structural reorder, 128), param-copy split x0/y0 (+11B, per old note), pnew swap (89).

   WIN kept: the 0x26da8-family CSE wall (target re-reads *node after the `*q = *node` store
   where -oneatx CSE-merges the loads) IS source-reachable -- cast the two post-store
   re-reads through `*(volatile unsigned short *)node` and only those loads split (fresh
   `cmp word [eax],0` + `mov r16,[eax]`), leaving all other codegen untouched. */
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

    ty = (short)y >> 8;
    tx = (short)x >> 8;
    if (tx < 1) x = 0x7e00;
    if (ty < 1) y = 0x5e00;
    if (tx >= 0x7f) x = 0x200;
    if (ty >= 0x5f) y = 0x200;
    z &= ~0xf000;

    pnew = g_grid_heads + (((short)(y & 0x7f00) >> 1) | (((short)x >> 8) & 0x7f));
    pold = g_grid_heads + ((((int)*(short *)(node + 4) >> 8) & 0x7f) | ((short)(*(unsigned short *)(node + 6) & 0x7f00) >> 1));
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
