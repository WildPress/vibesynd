/* frameless @ 0x26da8: remove a pool object from its spatial-grid doubly-linked list
   (the inverse of 0x26e18's head-insert). If the "in grid" flag (p[0xa]&4) is set:
   link = prev node (g_entity_pool + p[2]) if p has a prev, else the grid cell head g_grid_heads[idx]
   (idx from the high bytes of coords p[4],p[6]); set *link = p->next (p[0]); if a next
   exists, patch next->prev (g_entity_pool[next+2]) = p->prev; clear the flag.

   PARKED near-miss, now at 105/106 (cont. 21: two of the three original walls fell
   to proven levers � the volatile-deref cast `*(volatile unsigned short *)p` into a
   named ushort local splits the post-store re-read into the target's `mov cx,[eax];
   test cx,cx`, and the pointer statement `q = g_entity_pool + nx;` gives the unfolded
   `add edx,0x810e` + disp8). ONE instruction remains: target sign-extends the y-mask
   via `movsx edx,dx; mov ebx,edx` (in-place promote + copy), ours always merges to
   `movsx ebx,dx` � short/int/inline spellings + 4000 more cpermute variants all
   converge to the merged form. movsx-split wall, register-role family. */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];
void grid_unlink_object(unsigned char *p)
{
    if (p[0xa] & 4) {
        int ym = (short)(*(unsigned short *)(p + 6) & 0x7f00);
        int idx = (ym >> 1) | (((int)*(short *)(p + 4) >> 8) & 0x7f);
        unsigned short *link = &g_grid_heads[idx];
        unsigned short nx;
        if (*(unsigned short *)(p + 2) != 0)
            link = (unsigned short *)(g_entity_pool + *(unsigned short *)(p + 2));
        *link = *(unsigned short *)p;
        nx = *(volatile unsigned short *)p;
        if (nx != 0) {
            unsigned char *q = g_entity_pool + nx;
            *(unsigned short *)(q + 2) = *(unsigned short *)(p + 2);
        }
        p[0xa] &= 0xfb;
    }
}
