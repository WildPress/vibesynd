/* best_weapon_select_typed @ 0x37d08 (jump-table dispatcher, 19-entry table at 0x2a568).
 * Twin of best_weapon_select with an extra "wanted type" argument: walk the node's
 * carried-item chain (head id at node+0x3a, next link at item+0x1c,
 * item = g_entity_pool + id) and switch on the type byte item[0x19]. Weapon types
 * 2..9 keep the twin's rank/range best-weapon logic, but every case also
 * checks the item against the wanted type: an exact-type item with non-
 * negative +0x14 word is remembered separately and wins outright. Types
 * 0xb/0xd/0xe/0xf match on type alone; types 0x11..0x13 additionally demand
 * the +0x14 word be at least 10% of the per-type word at g_item_max_qty[type].
 * Returns the exact match's id if any, else the best weapon's id, else the
 * equipped item id at node+0x44 (0 if that item's +0x14 word is negative). */
extern unsigned char g_entity_pool[];
extern unsigned short g_item_max_qty[];

unsigned short best_weapon_select_typed(unsigned char *node, unsigned short dist, unsigned char type)
{
    unsigned short rank;
    unsigned short id;
    unsigned char *found;
    unsigned char *exact;
    unsigned char *item;
    unsigned char *q;
    short m1;

    m1 = -1;
    rank = 1;
    id = *(unsigned short *)(node + 0x3a);
    found = 0;
    exact = 0;
    if (id != 0) {
        for (;;) {
            if (id == 0)
                break;
            item = g_entity_pool + id;
            switch (item[0x19]) {
            case 2:
                if (rank <= 4 && *(short *)(item + 0x14) >= 0 && dist <= 0x500) {
                    rank = 4;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 4:
                if (rank <= 5 && *(short *)(item + 0x14) >= 0 && dist <= 0x400) {
                    rank = 5;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 5:
                if (rank <= 7 && *(short *)(item + 0x14) >= 0 && dist <= 0x700) {
                    rank = 7;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 6:
                if (rank <= 8 && *(short *)(item + 0x14) >= 0 && dist <= 0x900) {
                    rank = 8;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 7:
                if (rank <= 9 && *(short *)(item + 0x14) >= 0 && dist <= 0x1000) {
                    rank = 9;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 8:
                if (rank <= 6 && *(short *)(item + 0x14) >= 0 && dist <= 0x200) {
                    rank = 6;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 9:
                if (rank <= 5 && *(short *)(item + 0x14) >= 0 && dist <= 0x1800) {
                    rank = 5;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 3:
                if (rank <= 1 && *(short *)(item + 0x14) >= 0 && dist <= 0x1400) {
                    rank = 1;
                    found = item;
                }
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 1:
                if (type == item[0x19]) {
                    exact = item;
                    id = *(unsigned short *)(item + 0x1c);
                    continue;
                }
                break;
            case 0x11:
            case 0x12:
            case 0x13:
                if (type == item[0x19] && *(short *)(item + 0x14) > m1) {
                    if (*(short *)(item + 0x14) >= g_item_max_qty[item[0x19]] * 10 / 100)
                        exact = item;
                }
                break;
            case 0xb:
            case 0xd:
            case 0xe:
            case 0xf:
                if (type == item[0x19])
                    exact = item;
                id = *(unsigned short *)(item + 0x1c);
                continue;
            case 0xa:
            case 0xc:
            case 0x10:
            default:
                break;
            }
            id = *(unsigned short *)(item + 0x1c);
        }
    }
    if (exact != 0)
        return (unsigned short)(exact - g_entity_pool);
    if (found == 0) {
        q = g_entity_pool + *(unsigned short *)(node + 0x44);
        if (*(short *)(q + 0x14) < 0)
            return 0;
        return *(unsigned short *)(node + 0x44);
    }
    return (unsigned short)(found - g_entity_pool);
}
