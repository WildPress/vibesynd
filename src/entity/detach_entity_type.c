/* frameless @ 0x2fbc8: detach a pool entity from its "type" linked list and reset it.
   id = p[0x24] (next-in-list id); p[0x54]=0. If id==0, just stamp p[0x58]=p[0x19] and
   tail to FUN_0002d998. Else node = g_entity_pool + id; clear p[0xc]. If node is type 2
   (node[0x18]==2) relink via node[0x1c], else via node[0x22]; in either case if p has a
   prev (p[0x22]) patch that neighbour's node[0x24]. Then reset the record (p[0x55]=p[0x56],
   clear links p[0x24]/p[0x22], p[0x54]=0, clear flag bit0 of p[0xa]), re-place it via
   move_entity_xyz(p, x+1, y+1, z) and tail to FUN_0002d998.

   PARKED near-miss 209/211 (logic byte-identical; masked). Register-role wall on the entry
   compare: the target loads the id-test into DX (`66 8b 53 24` / `66 85 d2`) yet reloads the
   node base fresh from memory (`66 8b 43 24`); Watcom 9.5b either (a) keeps the compare inline
   -> it lands in DX but then CSE-reuses it for the node base (`66 89 d0`, 210B, node reload
   lost) or (b) uses a dying named temp for the compare -> the node base reloads correctly but
   the dead compare temp is allocated to AX (`66 8b 43 24` / `66 85 c0`). This file is form (b):
   only bytes 0x8 and 0x10 differ (compare register DX<->AX); everything after is identical.
   No source form (recipe sweep -oneatx/-or/-ot/-oi/-oc/-oa/-oe/-os/-od, char* CSE break,
   volatile, temp type/order, aliasing barriers) flips the dead compare temp DX vs AX. */
extern unsigned char g_entity_pool[];
extern void move_entity_xyz(unsigned char *p, int x, int y, int z);
extern void FUN_0002d998(unsigned char *p);

void detach_entity_type(unsigned char *p)
{
    unsigned char *node;
    unsigned char *q;
    unsigned short id;

    id = *(unsigned short *)(p + 0x24);
    p[0x54] = 0;
    if (id != 0) {
        node = g_entity_pool + *(unsigned short *)(p + 0x24);
        *(unsigned short *)(p + 0xc) = 0;
        if (node[0x18] == 2) {
            q = node;
            if (*(unsigned short *)(p + 0x22) != 0) {
                node = g_entity_pool + *(unsigned short *)(p + 0x22);
                *(unsigned short *)(node + 0x24) = *(unsigned short *)(p + 0x24);
            }
            *(unsigned short *)(q + 0x1c) = *(unsigned short *)(p + 0x22);
        } else {
            q = node;
            if (*(unsigned short *)(p + 0x22) != 0) {
                node = g_entity_pool + *(unsigned short *)(p + 0x22);
                *(unsigned short *)(node + 0x24) = *(unsigned short *)(p + 0x24);
            }
            *(unsigned short *)(q + 0x22) = *(unsigned short *)(p + 0x22);
        }
        p[0x55] = p[0x56];
        *(unsigned short *)(p + 0x24) = 0;
        *(unsigned short *)(p + 0x22) = 0;
        p[0x54] = 0;
        p[0xa] &= 0xfe;
        move_entity_xyz(p, (short)(*(short *)(p + 4) + 1), (short)(*(short *)(p + 6) + 1), *(short *)(p + 8));
        FUN_0002d998(p);
        return;
    }
    p[0x58] = p[0x19];
    FUN_0002d998(p);
}
