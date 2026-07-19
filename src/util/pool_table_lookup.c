/* Translation unit @ 0x377e8 -- two sibling pool accessors that SHARE a return-0 stub.
 *
 * 0x377e8 and 0x37818 are near-identical two-level table lookups (bounds check, then
 * return g_table[node[0x19]] else 0) that differ only in the table (g_a6ea vs g_a686).
 * In the shipped binary Watcom compiled them in ONE module and TAIL-MERGED the return-0
 * exits: 0x37818's out-of-range path does not emit its own `xor eax,eax; ret` -- it jumps
 * BACKWARD (jb 0x3780f) into 0x377e8's stub. That cross-function branch only exists if the
 * two functions live in the SAME translation unit (Watcom's cross-jump optimisation is
 * intra-object), so they MUST be compiled together -- hence this combined file.
 *
 * The merge requires 0x37818 to return `unsigned short` (not `unsigned char`): only then
 * is its return-0 stub `xor eax,eax; ret`, byte-identical to 0x377e8's, so Watcom can share
 * it. The normal path then ends `mov al,[tbl]; xor ah,ah; ret`, matching the target exactly.
 * Verified as a unit by tools/match_combo.py (whole-region reloc-aware byte match).
 *
 * Function ORDER matters: 0x377e8 (lower addr) must come first so it owns the shared stub.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char g_entity_pool[];
extern unsigned char g_11670;
extern unsigned char g_a6ea[];
extern unsigned char g_a686[];

/* @ 0x377e8: field = p->[0x44]; node = g_entity_pool + field; if node < &g_11670 -> 0;
   else return g_a6ea[node[0x19]]. Owns the shared return-0 stub at 0x3780f. */
unsigned short pool_table_lookup(unsigned char *p)
{
    unsigned char *node = g_entity_pool + *(unsigned short *)(p + 0x44);
    if (node >= &g_11670)
        return g_a6ea[node[0x19]];
    return 0;
}

/* @ 0x37818: same shape against g_a686; its return-0 tail-merges into 0x377e8's stub.
   Returns unsigned short so the shared stub is byte-identical (see unit header). */
unsigned short pool_table_lookup_a686(unsigned char *p)
{
    unsigned char *node = g_entity_pool + *(unsigned short *)(p + 0x44);
    if (node >= g_entity_pool + 0x9562)
        return g_a686[node[0x19]];
    return 0;
}
