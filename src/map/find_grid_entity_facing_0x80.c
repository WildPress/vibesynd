/* PARKED near-miss (NOT matched, 154/167 bytes, EDIT-DIST 17) -- spatial-grid proximity scan.
   NOTE: a STALE DUPLICATE src/entity/find_grid_entity_facing_0x80.c (wrong branch logic -- `!= 0x80` and
   `!= 0`, giving JE where the target has JNE/JBE, dist 48) used to shadow this file: the
   build's `find src -name find_grid_entity_facing_0x80.c | head -1` picked entity/ alphabetically before
   map/. Removed the entity duplicate so THIS correct version compiles (48 -> 17).
   Structure 100% correct (offsets, signedness, ==0x80 check, JBE health test, CMP DI,6,
   return width). Two §3 ties remain: (1) prologue param-load/init scheduling permutation
   ~10B at 0x04 (target loads esi[param_2] then ecx[param_3] then `sub esi`,`xor edi`; ours
   loads ecx first and `xor edi` before `sub esi`); (2) the grid-idx sign-extend
   `CWDE; MOV EDX,EAX` (target) vs `MOVSX EDX,AX` (ours), same encoding-tie class as 0x34088.
   Swapping the OR operands, a `short` row-part temp, and split loop-init were all tried:
   regress or byte-identical. Near-twins 0x33b88/0x33c38/0x33cf8 share this template/wall.
   Proposed name: find_grid_entity_facing_0x80 (mirror of find_grid_entity_facing_0xc0, which faces 0xc0). */
/* frameless @ 0x33db8: spatial-grid proximity scan. Walk 6 grid rows starting at
   (param_2 - 0x100), stepping +0x100 per row. For each row build the 128x128
   grid-cell index from the high bits of iVar3 (>>1) and of param_1 (>>8), and walk
   the object chain rooted at g_grid_heads[idx] (node = g_entity_pool + id, next-link at node+0).
   Return 1 for the first node that is type 2 (node[0x18]==2), on the same level as
   param_3 (high byte of node[8] == high byte of param_3), has a secondary link
   node[0x1c]!=0, faces 0x80 (node[0x1a]==0x80), and whose linked node has a nonzero
   value byte at +0x54. Return 0 after 6 rows with no hit. Near-twin of find_grid_entity_facing_0xc0. */
/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): two divergences, both inert. (1) Prologue param-load
   order permutation -- target `mov esi,[esp+0x14]; mov ecx,[esp+0x18]; sub esi,0x100; xor edi,edi`
   vs ours `mov ecx,[esp+0x18]; mov esi,[esp+0x14]; xor edi,edi; sub esi,0x100` -- same loads to the
   same registers (esi=param_2, ecx=param_3), same ops, reordered. (2) grid-index sign-extend
   `cwde; mov edx,eax` (target) vs `movsx edx,ax` (ours), same 16->32 result. Every struct offset
   (0x18, 8, 0x1c, 0x1a, 0x54), constant (0x100, sar-1, sar-8, 0x7f mask, 0xff00, 0xffff, 2, 6, 0x80),
   and branch condition (jne/je/jbe/jb) is byte-identical. Same value returned for all inputs. */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];

int find_grid_entity_facing_0x80(short param_1, int param_2, short param_3)
{
    int iVar3;
    unsigned short row;
    for (iVar3 = param_2 - 0x100, row = 0; row < 6; iVar3 += 0x100, row++) {
        unsigned short head =
            g_grid_heads[((short)(iVar3 & 0x7f00) >> 1) | ((param_1 >> 8) & 0x7f)];
        while (head != 0) {
            unsigned char *node = g_entity_pool + head;
            if (node[0x18] == 2 &&
                (0xff00 & *(short *)(node + 8)) == (param_3 & 0xff00) &&
                *(unsigned short *)(node + 0x1c) != 0 &&
                node[0x1a] == 0x80) {
                unsigned char *n2 = g_entity_pool + *(unsigned short *)(node + 0x1c);
                if (n2[0x54] > 0)
                    return 1;
            }
            head = *(unsigned short *)node;
        }
    }
    return 0;
}
