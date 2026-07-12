/* frameless @ 0x26da8: remove a pool object from its spatial-grid doubly-linked list
   (the inverse of 0x26e18's head-insert). If the "in grid" flag (p[0xa]&4) is set:
   link = prev node (g_810e + p[2]) if p has a prev, else the grid cell head g_10e[idx]
   (idx from the high bytes of coords p[4],p[6]); set *link = p->next (p[0]); if a next
   exists, patch next->prev (g_810e[next+2]) = p->prev; clear the flag.

   PARKED near-miss (NOT matched; logic correct, idx build matches). Wall at 84/106:
   the target is LESS optimised than -oneatx gives us in three spots -- it re-reads
   *p into a fresh reg (we CSE-merge it), sign-extends the b-part via `movsx edx,dx;
   mov ebx,edx` (we do `movsx ebx,dx` directly), and forms next->prev via `add edx,
   0x810e; [edx+2]` (we fold to `[edx+0x8110]`). Lighter recipes (-ot/-oat/-os/-oa) and
   6000 cpermute variants don't split these. Same grid-family opt/CSE wall as 0x26e18. */
extern unsigned char g_810e[];
extern unsigned short g_10e[];
void FUN_00026da8(unsigned char *p)
{
    if (p[0xa] & 4) {
        int idx = ((short)(*(unsigned short *)(p + 6) & 0x7f00) >> 1) |
                  (((int)*(short *)(p + 4) >> 8) & 0x7f);
        unsigned short *link = &g_10e[idx];
        if (*(unsigned short *)(p + 2) != 0)
            link = (unsigned short *)(g_810e + *(unsigned short *)(p + 2));
        *link = *(unsigned short *)p;
        if (*(unsigned short *)p != 0)
            *(unsigned short *)(g_810e + *(unsigned short *)p + 2) = *(unsigned short *)(p + 2);
        p[0xa] &= 0xfb;
    }
}
