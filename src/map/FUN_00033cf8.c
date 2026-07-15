/* frameless @ 0x33cf8: grid-hit scan along y (twin of 0x33c38 which scans x).
   Scans 6 tiles at decreasing y starting from y+0x100: for each, walks the g_grid_heads
   grid-head chain in the g_entity_pool pool looking for a node with type[0x18]==2,
   matching (field8 & 0xff00) == (k & 0xff00), a nonzero link at +0x1c, flag
   [0x1a]==0, and the linked node's health [0x54] > 0. Returns 1 on hit, else 0. */
extern unsigned short g_grid_heads[];
extern unsigned char g_entity_pool[];

int FUN_00033cf8(short x, short y, short k)
{
    unsigned short i;
    unsigned short id;
    unsigned char *p;
    unsigned char *q;

    y += 0x100;
    for (i = 0; i < 6; i++) {
        id = g_grid_heads[((short)(y & 0x7f00) >> 1) | ((x >> 8) & 0x7f)];
        while (id) {
            p = g_entity_pool + id;
            if (p[0x18] == 2
                && (*(short *)(p + 8) & 0xff00) == (k & 0xff00)
                && *(unsigned short *)(p + 0x1c) != 0
                && p[0x1a] == 0) {
                q = g_entity_pool + *(unsigned short *)(p + 0x1c);
                if (q[0x54] > 0)
                    return 1;
            }
            id = *(unsigned short *)p;
        }
        y -= 0x100;
    }
    return 0;
}
