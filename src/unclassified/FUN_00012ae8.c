/* PARKED near-miss @ 0x12ae8 -- 392/443 bytes, first diff 0x5f, size match
   443/443, all fixup sites aligned. Register-role/spill-slot wall, same family
   as sibling 0x128b8.

   Semantics: find an overlapping related ped. Scans the 2x2 g_grid_heads grid cells
   whose top-left is the tile of (x - w/2 - 0x80, y - h/2 - 0x80), walking each
   cell's id chain (bounded at 0x400 nodes). A node matches when it is not self,
   type[0x18]==1, flag bits (node[0xb] & 1) and (node[0xa] & 1) clear, its x/y
   coords overlap the box centred on (x,y) inflated by w/2 / h/2 on both sides,
   z - zr <= nz <= z + zr, and an ownership relation holds: same owner word
   (+0x20), node's owner == self's id, or node's id == self's owner.
   Early-out 0 when y - h/2 - 0x80 <= 0. Returns node pointer or 0.
   Sibling: FUN_000128b8 (3x3 variant). Recipe: -4s -oneatx -zp8 -s -zq.

   WALL (remaining diffs, all allocator-internal):
   (1) setup interleave + spill slots: ours stores self_id before iy=0 and
       assigns iy->8/ix->0xc/i->0x10 where target has ix->8/i->0xc/iy->0x10;
       decl-order flips are byte-inert (confirmed, as on 0x338d8).
   (2) guard1-x temp pick: ours copies c into EBP (cmp reversed to JL) where
       target copies into EDI (JG); the IDENTICALLY-spelled guard1-y comes out
       in the target's EDI/JG form -- pure positional rotation. Swapping the
       operand order flips roles but reverses the cmp sense (anti-correlated,
       exactly like sibling 0x128b8's parked wall).
   (3) z-block scratch rotation: ours z->EDX/zr->ECX/nz->EAX vs target
       z->ECX/zr->EAX/nz->EDX after byte-identical preceding code.
   Levers that WERE load-bearing here: inline (CSE) y-part for the guard so dy
   lives EAX->EBX; named `row` for the y-half of the index so the OR lands in
   EAX (or eax,ebx); named `c` coord temp to evict self from ECX into ESI
   (which also fixed k-init via ECX); `c += w/2` compound form to get the
   in-place add ecx,eax in guard 2; INLINE node[0x18] (not a named t) to home
   the type byte in AH (cmp ah,1, 3B) instead of AL (cmp al,1, 2B). */
extern unsigned short g_grid_heads[];
extern unsigned char g_entity_pool[];

unsigned char *FUN_00012ae8(unsigned char *self, short x, short y, short z,
                            short w, short h, short zr)
{
    unsigned short *p;
    int self_id;
    unsigned short ix;
    unsigned short i;
    unsigned short iy;
    unsigned short id;
    unsigned char *node;
    int row;
    int c;

    if (y - h / 2 - 0x80 > 0) {
        row = ((y - h / 2 - 0x80) & 0x7f00) >> 1;
        p = g_grid_heads + ((((x - w / 2 - 0x80) >> 8) & 0x7f) | row);
        self_id = self - g_entity_pool;
        for (iy = 0; iy < 2; iy++) {
            for (ix = 0; ix < 2; ix++) {
                i = 0;
                id = *p;
                if (id != 0) {
                    do {
                        node = g_entity_pool + id;
                        if (node == self)
                            goto skip;
                        if (node[0x18] != 1)
                            goto skip;
                        if ((unsigned char)(node[0xb] & node[0x18]))
                            goto skip;
                        if ((unsigned char)(node[0xa] & node[0x18]))
                            goto skip;
                        c = *(short *)(node + 4);
                        if (c - w / 2 > x + w / 2)
                            goto skip;
                        c += w / 2;
                        if (x - w / 2 > c)
                            goto skip;
                        c = *(short *)(node + 6);
                        if (c - h / 2 > y + h / 2)
                            goto skip;
                        c += h / 2;
                        if (y - h / 2 > c)
                            goto skip;
                        if (z + zr < *(short *)(node + 8))
                            goto skip;
                        if (z > zr + *(short *)(node + 8))
                            goto skip;
                        if (*(unsigned short *)(self + 0x20) == *(unsigned short *)(node + 0x20))
                            goto found;
                        if (*(unsigned short *)(node + 0x20) == (unsigned short)self_id)
                            goto found;
                        if ((unsigned short)(node - g_entity_pool) != *(unsigned short *)(self + 0x20))
                            goto skip;
                    found:
                        return node;
                    skip:
                        i++;
                        id = *(unsigned short *)node;
                        if (i >= 0x400)
                            break;
                    } while (id != 0);
                }
                p++;
            }
            p += 0x7e;
        }
    }
    return 0;
}
