/* FUN_00037ad8 @ 0x37ad8 (jump-table dispatcher, 19-entry table at 0x2a340).
 * Best-weapon-in-range selector: walk node's carried-item chain (head id at
 * node+0x3a, next link at item+0x1c, item = g_810e + id). For each item,
 * switch on the type byte item[0x19]; weapon types 2..9 each carry a
 * (rank, max-range) pair -- take the item when its rank >= the current best
 * rank, its +0x14 word (ammo/hp) is >= 0, and dist is within range:
 *   type 2 -> rank 4, range 0x500     type 7 -> rank 9, range 0x1000
 *   type 4 -> rank 5, range 0x400     type 8 -> rank 6, range 0x200
 *   type 5 -> rank 7, range 0x700     type 9 -> rank 5, range 0x1800
 *   type 6 -> rank 8, range 0x900     type 3 -> rank 1, range 0x1400
 * All other types (1, 0xa..0x13) just advance. Returns the winning item's id;
 * if none found, falls back to the equipped item id at node+0x44 (0 if that
 * item's +0x14 word is negative). */
extern unsigned char g_810e[];

unsigned short FUN_00037ad8(unsigned char *node, unsigned short dist)
{
    unsigned short rank;
    unsigned short id;
    unsigned char *found;
    unsigned char *item;
    unsigned char *q;

    rank = 1;
    id = *(unsigned short *)(node + 0x3a);
    found = 0;
    if (id != 0) {
        for (;;) {
            if (id == 0)
                break;
            item = g_810e + id;
            switch (item[0x19]) {
            case 2:
                if (rank <= 4 && *(short *)(item + 0x14) >= 0 && dist <= 0x500) {
                    rank = 4;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 4:
                if (rank <= 5 && *(short *)(item + 0x14) >= 0 && dist <= 0x400) {
                    rank = 5;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 5:
                if (rank <= 7 && *(short *)(item + 0x14) >= 0 && dist <= 0x700) {
                    rank = 7;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 6:
                if (rank <= 8 && *(short *)(item + 0x14) >= 0 && dist <= 0x900) {
                    rank = 8;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 7:
                if (rank <= 9 && *(short *)(item + 0x14) >= 0 && dist <= 0x1000) {
                    rank = 9;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 8:
                if (rank <= 6 && *(short *)(item + 0x14) >= 0 && dist <= 0x200) {
                    rank = 6;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 9:
                if (rank <= 5 && *(short *)(item + 0x14) >= 0 && dist <= 0x1800) {
                    rank = 5;
                    found = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 3:
                if (rank <= 1 && *(short *)(item + 0x14) >= 0 && dist <= 0x1400) {
                    rank = 1;
                    found = item;
                }
                break;
            case 1:
            case 0xa:
            case 0xb:
            case 0xc:
            case 0xd:
            case 0xe:
            case 0xf:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            default:
                break;
            }
            id = *(unsigned short *)(item + 0x1c);
        }
    }
    if (found == 0) {
        q = g_810e + *(unsigned short *)(node + 0x44);
        if (*(short *)(q + 0x14) < 0)
            return 0;
        return *(unsigned short *)(node + 0x44);
    }
    return (unsigned short)(found - g_810e);
}
