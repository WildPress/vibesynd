/* PARKED near-miss (110/130 masked bytes) @ 0x26e18 -- NOT byte-matched.
   Head-insert of pool object p into spatial-grid cell g_10e[idx], idx built from
   the high bytes of coords a,b; maintains the doubly-linked list (p->next=head,
   head->prev=p_id, grid=p_id) then stores the coords. Logic is correct and the
   whole tail matches; it stalls on a register-role choice the compiler makes and
   no source form recovers: the target keeps idx in EDX (so the grid address is
   materialised in place via `add edx,edx; add edx,0x10e` and reused for read+write)
   and p_id in EBX; every C spelling we tried puts idx in EBX, forcing a `lea` and a
   2-byte-longer block. Same class as the 0x33fb8 / 0x34118-pre register walls. */
extern unsigned char g_810e[];
extern unsigned short g_10e[];
void FUN_00026e18(unsigned char *p, unsigned short a, unsigned short b, unsigned short d)
{
    if (!(p[0xa] & 4)) {
        unsigned int bp = (b & 0x7f00) >> 1;
        unsigned int idx = ((a >> 8) & 0x7f) | bp;
        unsigned short pid = (unsigned short)(p - g_810e);
        unsigned short *cell = &g_10e[idx];
        unsigned short head;
        *(unsigned short *)(p + 2) = 0;
        head = *cell;
        *(unsigned short *)p = head;
        if (head != 0)
            *(unsigned short *)(g_810e + head + 2) = pid;
        *cell = pid;
        p[0xa] |= 4;
    }
    *(unsigned short *)(p + 4) = a;
    *(unsigned short *)(p + 6) = b;
    *(unsigned short *)(p + 8) = d;
}
