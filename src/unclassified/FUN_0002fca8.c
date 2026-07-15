/* @ 0x2fca8: MATCHED (RELOC-AWARE YES, -4s -oneatx -zp8 -s -zq, 438B).
   Levers that closed it: named pointer statements for every g_entity_pool + id deref
   (add+disp8, no symbol fold); named int accumulator `idx = ...; idx += ...;`
   so the tile-index sum homes in EBP; named `base = g_map_cols; slot = base + idx;`
   to force the a1 load before the lea (anti-correlated with the plain
   `p[0xa] &= 0xf7` form in the 9/10-else block — &g_map_cols[idx] only compiled
   load-first while a byte temp existed there); `(int)*slot` cast so the tile
   deref emits `add eax,[ebp]` instead of a SIB-folded cmp; INLINE r[0x54] and
   q[0x1a] (CSE) rather than named byte temps so they land in DH/BL.

   pool-A entity arrival/attach step (sibling of 0x2fbc8). p[0x54]=0,
   node = g_entity_pool + p[0x24] (owner id), clear p[0xc], keep a second copy w of
   node for the type checks. If p has a pending id at +0x20 whose node's +0x24
   link is empty, probe the blocked-tile map: slot = &g_map_cols[(y%0x6000/0x100
   << 7) + (x&0xff00)/0x100]; if the tile byte at *slot + z/0x80 is not type 2,
   fail: p[0x19]=7 (anim), p[0x58]=0x1e (timer), clear flag bit3 of p[0xa],
   return. If the owner is a type-2 node in state 9/10 and p's target coords
   (+0x34/+0x36) differ from the owner's position: walk the +0x20 chain to its
   tail q; if q has a +0x1c neighbour whose health byte (+0x54) is zero, set
   p's facing (+0x1a) to 0x40 when q's facing is 0/0x80 else to the (zero)
   health value, stamp p[0x58]=7 and re-dispatch via FUN_2d998. If the owner is
   type 2 but NOT in state 9/10: p[0x55]=node[0x28], clear flag bit3, p[0x58]=
   p[0x19], re-dispatch via FUN_2d998. Otherwise walk p's +0x24 chain to the
   first type-2 ancestor and re-place p at its coords via FUN_26c78. */
extern unsigned char g_entity_pool[];
extern char **g_map_cols;
extern void FUN_0002d998(unsigned char *p);
extern void move_entity_xyz(unsigned char *node, int x, int y, int z);

void FUN_0002fca8(unsigned char *p)
{
    unsigned char f;
    unsigned char *node;
    unsigned char *w;
    unsigned char *q;
    unsigned char *m;
    unsigned char *r;
    unsigned short id;

    p[0x54] = 0;
    node = g_entity_pool + *(unsigned short *)(p + 0x24);
    *(unsigned short *)(p + 0xc) = 0;
    w = node;
    if (*(unsigned short *)(p + 0x20) != 0) {
        m = g_entity_pool + *(unsigned short *)(p + 0x20);
        if (*(unsigned short *)(m + 0x24) == 0) {
            char **slot;
            char **base;
            int idx;
            idx = *(short *)(p + 6) % 0x6000 / 0x100 << 7;
            idx += (*(short *)(p + 4) & 0xff00) / 0x100;
            base = g_map_cols;
            slot = base + idx;
            if (*(unsigned char *)(*(short *)(p + 8) / 0x80 + (int)*slot) != 2) {
                p[0x19] = 7;
                p[0x58] = 0x1e;
                p[0xa] &= 0xf7;
                return;
            }
        }
    }
    if (w[0x18] == 2 && (w[0x19] == 9 || w[0x19] == 0xa)) {
        if (*(unsigned short *)(p + 0x34) != *(unsigned short *)(node + 4)
            || *(unsigned short *)(p + 0x36) != *(unsigned short *)(node + 6)) {
            id = *(unsigned short *)(node + 0x20);
            while (id != 0) {
                q = g_entity_pool + id;
                id = *(unsigned short *)(q + 0x20);
            }
            if (*(unsigned short *)(q + 0x1c) != 0) {
                r = g_entity_pool + *(unsigned short *)(q + 0x1c);
                if (r[0x54] == 0) {
                    if (q[0x1a] == 0 || q[0x1a] == 0x80)
                        p[0x1a] = 0x40;
                    else
                        p[0x1a] = r[0x54];
                    p[0x58] = 7;
                    FUN_0002d998(p);
                    return;
                }
            }
        }
    }
    if (w[0x18] == 2 && w[0x19] != 9 && w[0x19] != 0xa) {
        p[0x55] = node[0x28];
        p[0xa] &= 0xf7;
        p[0x58] = p[0x19];
        FUN_0002d998(p);
        return;
    }
    id = *(unsigned short *)(p + 0x24);
    while (id != 0) {
        node = g_entity_pool + id;
        if (node[0x18] == 2)
            break;
        id = *(unsigned short *)(node + 0x24);
    }
    move_entity_xyz(p, *(short *)(node + 4), *(short *)(node + 6),
                 *(short *)(node + 8));
}
