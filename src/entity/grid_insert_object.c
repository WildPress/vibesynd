/* MATCHED @ 0x26e18 (130/130 reloc-aware, -4s -oneatx -zp8 -s -zq).
   Head-insert of pool object p into spatial-grid cell g_grid_heads[idx], idx built from
   the high bytes of coords a,b; maintains the doubly-linked list (p->next=head,
   head->prev=p_id, grid=p_id) then stores the coords.

   cont.21 retry un-parked the old 110/130 "idx in EDX" register wall with TWO
   levers: (1) pointer statement `q = g_entity_pool + head;` (proven on sibling 0x26da8)
   gives the unfolded `add ebx,0x810e` + disp8 store instead of the symbol folded
   into a disp32; (2) STATEMENT ORDER: computing `cell = &g_grid_heads[idx]` BEFORE
   `pid = p - g_entity_pool` lets idx die straight into the address calc, so it stays in
   scratch EDX (`or edx,ebx; add edx,edx; add edx,0x10e`) and pid lands in EBX --
   the exact target roles. (Swapping the OR operands instead flips the a/b
   evaluation order upstream, NOT the destination -- wrong lever here.) */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];
void grid_insert_object(unsigned char *p, unsigned short a, unsigned short b, unsigned short d)
{
    if (!(p[0xa] & 4)) {
        unsigned int bp = (b & 0x7f00) >> 1;
        unsigned int idx = ((a >> 8) & 0x7f) | bp;
        unsigned short *cell = &g_grid_heads[idx];
        unsigned short pid = (unsigned short)(p - g_entity_pool);
        unsigned short head;
        *(unsigned short *)(p + 2) = 0;
        head = *cell;
        *(unsigned short *)p = head;
        if (head != 0) {
            unsigned char *q = g_entity_pool + head;
            *(unsigned short *)(q + 2) = pid;
        }
        *cell = pid;
        p[0xa] |= 4;
    }
    *(unsigned short *)(p + 4) = a;
    *(unsigned short *)(p + 6) = b;
    *(unsigned short *)(p + 8) = d;
}
