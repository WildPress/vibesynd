/* frameless @ 0x128b8: find a live ped (pool-A type 1) near (x,y,z).
   Scans the 3x3 g_10e grid cells whose top-left is the tile of
   (x - rx/2 - 0x100, y - ry/2 - 0x100), walking each cell's id chain
   (bounded at 0x400 nodes). A node matches when type[0x18]==1, flag
   bit (node[0xb] & type) clear, (link word +0x1c & mask) != 0, and its
   coords fall inside the box x +/- (rx/2 + 0x20), y +/- (ry/2 + 0x20),
   z - 0x100 <= nz <= z + rz. First param is unused. Returns the node
   pointer, or 0. */
extern unsigned short g_10e[];
extern unsigned char g_810e[];

unsigned char *FUN_000128b8(int unused, unsigned short mask, short x, short y,
                            short z, int rx, int ry, int rz)
{
    int cy;
    int hry;
    int hrx;
    unsigned short *p;
    unsigned short iy;
    unsigned short ix;
    unsigned short i;
    unsigned short id;
    unsigned char *node;
    unsigned char t;

    hry = ry / 2;
    cy = y - hry;
    p = g_10e + ((((cy - 0x100) & 0x7f00) >> 1) | (((x - rx / 2 - 0x100) >> 8) & 0x7f));
    hrx = rx / 2;
    for (iy = 0; iy < 3; iy++) {
        for (ix = 0; ix < 3; ix++) {
            i = 0;
            id = *p;
            if (id != 0) {
                do {
                    node = g_810e + id;
                    t = node[0x18];
                    if (t != 1)
                        goto skip;
                    if ((unsigned char)(node[0xb] & t))
                        goto skip;
                    if (!(*(unsigned short *)(node + 0x1c) & mask))
                        goto skip;
                    if (*(short *)(node + 4) + 0x20 < x - hrx)
                        goto skip;
                    if (*(short *)(node + 4) - 0x20 > x + hrx)
                        goto skip;
                    if (y - hry > *(short *)(node + 6) + 0x20)
                        goto skip;
                    if (*(short *)(node + 6) - 0x20 > y + hry)
                        goto skip;
                    if (*(short *)(node + 8) + 0x100 < z)
                        goto skip;
                    if (*(short *)(node + 8) <= z + rz)
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
        p += 0x7d;
    }
    return 0;
}
