/* frameless @ 0x33c38: grid-hit scan along x (twin of 0x33cf8 which scans y).
   Scans 6 tiles at decreasing x starting from x+0x100: for each, walks the g_10e
   grid-head chain in the g_810e pool looking for a node with type[0x18]==2,
   matching (field8 & 0xff00) == (k & 0xff00), a nonzero link at +0x1c, flag
   [0x1a]==0x40, and the linked node's health [0x54] > 0. Returns 1 on hit, 0. */
extern unsigned short g_10e[];
extern unsigned char g_810e[];

int FUN_00033c38(short x, short y, short k)
{
    unsigned short i;
    unsigned short id;
    unsigned char *p;
    unsigned char *q;

    x += 0x100;
    for (i = 0; i < 6; i++) {
        id = g_10e[((short)(y & 0x7f00) >> 1) | ((x >> 8) & 0x7f)];
        while (id) {
            p = g_810e + id;
            if (p[0x18] == 2
                && (*(short *)(p + 8) & 0xff00) == (k & 0xff00)
                && *(unsigned short *)(p + 0x1c) != 0
                && p[0x1a] == 0x40) {
                q = g_810e + *(unsigned short *)(p + 0x1c);
                if (q[0x54] > 0)
                    return 1;
            }
            id = *(unsigned short *)p;
        }
        x -= 0x100;
    }
    return 0;
}
