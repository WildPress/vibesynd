/* PARKED near-miss (NOT matched) -- exact-size 171/171, all 3 relocs aligned, and
   the entire tail from obj offset 0x37 (byte 55) to the end is byte-IDENTICAL.
   The whole divergence is the leading 49-byte prologue+index block (same length,
   just re-scheduled/re-allocated): (1) param-load order -- Watcom insists on
   loading param_3 into ECX first, target loads param_1(esi),param_3(ecx),param_2(eax);
   (2) grid-index register role -- target puts the col part ((short)step>>8&0x7f) in
   EAX and the row part ((short)hi>>1) in EDX; Watcom fixes the memory operand (hi,
   re-read from [esp]) into EAX and the register operand (step, in ESI) into EDX,
   independent of source operand order; (3) mask peephole -- `and eax,0x7f00` vs the
   target's byte-wise `and ah,0x7f; xor al,al`. Same §3 register-role + prologue-
   scheduling wall documented on the identical-template twin find_grid_entity_facing_0x80. */
/* frameless @ 0x33b88: spatial-grid proximity scan (mirror of find_grid_entity_facing_0x80).
   Walk 6 grid columns starting at (param_1 - 0x100), stepping +0x100 per row.
   The row part (param_2 & 0x7f00) >> 1 is loop-invariant and hoisted into a
   local; the col part ((short)step >> 8) & 0x7f varies per row. Walk the object
   chain rooted at g_grid_heads[idx] (node = g_entity_pool + id, next-link at node+0). Return 1
   for the first node that is type 2 (node[0x18]==2), on the same level as param_3
   (high byte of node[8] == high byte of param_3), has a secondary link
   node[0x1c]!=0, faces 0xc0 (node[0x1a]==0xc0), and whose linked node has a
   nonzero value byte at +0x54. Return 0 after 6 rows with no hit. */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];

int FUN_00033b88(int param_1, int param_2, short param_3)
{
    int step;
    int hi;
    unsigned short row;

    step = param_1 - 0x100;
    hi = param_2 & 0x7f00;
    for (row = 0; row < 6; row++, step += 0x100) {
        unsigned short head =
            g_grid_heads[(((short)step >> 8) & 0x7f) | ((short)hi >> 1)];
        while (head != 0) {
            unsigned char *node = g_entity_pool + head;
            if (node[0x18] == 2 &&
                (0xff00 & *(short *)(node + 8)) == (param_3 & 0xff00) &&
                *(unsigned short *)(node + 0x1c) != 0 &&
                node[0x1a] == 0xc0) {
                unsigned char *n2 = g_entity_pool + *(unsigned short *)(node + 0x1c);
                if (n2[0x54] > 0)
                    return 1;
            }
            head = *(unsigned short *)node;
        }
    }
    return 0;
}
