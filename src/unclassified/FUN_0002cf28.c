/* PARKED near-miss (NOT matched) -- 240/262 with EXACT length 262 (TRUE size;
 * manifest says 184, undercounted: code runs 0x2cf28..0x2d02d). All relocs,
 * the whole entry/slot/ped block, both helper-call guards, the loop skeleton,
 * and the entire 20-entry switch (15 push-const bodies, cross-jumped call
 * tail) are byte-identical. Three allocator/scheduler residues (stable across
 * 7 spellings; do-while, for-update, comma-update, shared-local, unnamed-temp
 * all emit the same bytes): (1) outer type byte homed BL vs target DL (3 x 1
 * byte); (2) loop switch byte AL + `and eax,0xff` vs target CH + `xor eax,
 * eax; mov al,ch` (~10 bytes at obj 0x91..0x9f; unnamed temp moves it to CL,
 * one digit off); (3) loop tail scheduled `mov ax,[ebx+0x1c]; add esi,0x12`
 * vs target `add esi,0x12; mov ax,[ebx+0x1c]` (486 AGI gap-fill; -3s changes
 * the whole codegen style so the target is NOT -3s). Same register-role wall
 * family as playbook 3; cpermute confirmed useless on this class (cont.21).
 *
 * @ 0x2cf28 (jump-table dispatcher, 20-entry table at manifest
 * 0x2cecc, jmp literal 0x1f784). HUD inventory-slot state refresh for the
 * selected agent (g_cur_player): slot = g_agent_slots[n*0x417] + (signed char)g_agent_slots[n*0x417+1],
 * ped = g_pool_a + slot*0x5c. If ped type is 0x18/0x19, or (ped flag +0xb bit 0
 * set and g_df52[b*0xb] < 2), reset the record table via FUN_0002c468(); if
 * type == 0xa refresh via FUN_0002c4e8(). Then walk the carried-item chain
 * (head id at ped+0x3a, next at item+0x1c, item = g_entity_pool + id), advancing a
 * record cursor (0x5256, stride 0x12) per item, and for each item type 1..0x13
 * feed FUN_0002d038(ped, rec, item, sprite):
 *   1..0xc -> 0x4b + (t-1)*4;  0xd/0xe/0xf -> 0x83;  0x10 -> 0x87;
 *   0x11/0x12/0x13 -> 0x8b;  type 0 and >0x13 -> no call (cursor still advances).
 */
extern short g_cur_player;
extern unsigned char g_agent_slots[];
extern unsigned char g_df52[];
extern unsigned char g_pool_a[];
extern unsigned char g_entity_pool[];
extern short g_5256[];

void FUN_0002c468(void);
void FUN_0002c4e8(void);
void FUN_0002d038(unsigned char *p, short *rec, int item, short spr);

void FUN_0002cf28(void)
{
    int off = g_cur_player * 0x417;
    int b = (signed char)g_agent_slots[off + 1];
    int slot = g_agent_slots[off] + b;
    unsigned char *ped = g_pool_a + slot * 0x5c;
    unsigned char type = ped[0x19];
    unsigned short id;
    short *rec;
    unsigned char *item;

    if (type == 0x18 || type == 0x19 ||
        ((ped[0xb] & 1) && g_df52[b * 0xb] < 2))
        FUN_0002c468();
    if (ped[0x19] == 0xa)
        FUN_0002c4e8();
    id = *(unsigned short *)(ped + 0x3a);
    rec = g_5256;
    if (id != 0) {
        do {
            item = g_entity_pool + id;
            type = item[0x19];
            switch (type) {
            case 1:
                FUN_0002d038(ped, rec, (int)item, 0x4b);
                break;
            case 2:
                FUN_0002d038(ped, rec, (int)item, 0x4f);
                break;
            case 3:
                FUN_0002d038(ped, rec, (int)item, 0x53);
                break;
            case 4:
                FUN_0002d038(ped, rec, (int)item, 0x57);
                break;
            case 5:
                FUN_0002d038(ped, rec, (int)item, 0x5b);
                break;
            case 6:
                FUN_0002d038(ped, rec, (int)item, 0x5f);
                break;
            case 7:
                FUN_0002d038(ped, rec, (int)item, 0x63);
                break;
            case 8:
                FUN_0002d038(ped, rec, (int)item, 0x67);
                break;
            case 9:
                FUN_0002d038(ped, rec, (int)item, 0x6b);
                break;
            case 0xa:
                FUN_0002d038(ped, rec, (int)item, 0x6f);
                break;
            case 0xb:
                FUN_0002d038(ped, rec, (int)item, 0x73);
                break;
            case 0xc:
                FUN_0002d038(ped, rec, (int)item, 0x77);
                break;
            case 0xd:
            case 0xe:
            case 0xf:
                FUN_0002d038(ped, rec, (int)item, 0x83);
                break;
            case 0x10:
                FUN_0002d038(ped, rec, (int)item, 0x87);
                break;
            case 0x11:
            case 0x12:
            case 0x13:
                FUN_0002d038(ped, rec, (int)item, 0x8b);
                break;
            case 0:
            default:
                break;
            }
            rec += 9;
            id = *(unsigned short *)(item + 0x1c);
        } while (id != 0);
    }
}
