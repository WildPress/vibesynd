/* PARKED near-miss (NOT matched) -- EDIT-DIST 77 (was 87), TRUE size 310 (manifest
 * says 150, undercounted: carve stops at the jmp CS:[...] -- code runs to the
 * ret at 0x33fad). Entry block, tile lookup tail, switch, and ALL EIGHT case
 * bodies are byte-identical. The residue is now HALF of the slot-formation
 * register triangle plus one perturbed divisor:
 *   - FIXED (this pass): `int zz = z;` hoists z into EDI ahead of the row home,
 *     so `row` now lands in ECX (target's `mov ecx,eax` at obj 0x30) instead of
 *     EDI, and the movsx-z / row-shift order now matches target. That removed
 *     the 3 row reg-digit diffs and re-ordered the z load correctly (-10 dist).
 *   - SIDE COST of zz: with z live in EDI across the whole body the allocator
 *     colours the 0x6000 modulo divisor into ECX (b9/f7f9) instead of target's
 *     ESI (be/f7fe) -- 2 bytes. The old xs-holds-0x6000-then-x trick no longer
 *     forces ESI once z occupies EDI; x still lands in ESI for the push-esi
 *     case bodies, so only the divisor slips.
 *   - STILL WRONG: the slot load -- ours `lea eax,[eax*4]; mov ecx,[g_map_cols]
 *     (8b0d,6B)` vs target `lea ecx,[eax*4]; mov eax,[g_map_cols](A1,5B)`. This
 *     is the +1 body byte (311 vs 310) that alone blocks the jump-table-aware
 *     match. It is the base->A1-EAX / scaled->lea-ECX half of the triangle:
 *     Watcom will only evacuate the scaled index out of EAX (freeing EAX for the
 *     1-byte A1 pointer load) as one coupled decision with row->ECX, and we can
 *     reach the row half but not the slot half. WALL: allocator-internal.
 * TRIED and reverted this pass: operand swap (no change); int-address slot
 * `*(int*)((int)g_map_cols + index*4)` folds to `add ecx,[mem]` (86, worse);
 * `g_map_cols[index]` array spelling (identical 77); materialising the pointer
 * in a `char **gmc` local (83, worse); dropping volatile (103, worse). LEVERS
 * KEPT: volatile on g_map_cols; the xs int-local for x->ESI; `int zz = z`.
 *
 * @ 0x33e78 (jump-table dispatcher, 6-entry table at manifest 0x33e60, jmp
 * literal 0x26718). Map-tile hit dispatcher: same map/tile lookup as
 * map_passability_check (row = (y % 0x6000)/256, col = (x & 0xff00)/256, slot =
 * g_map_cols + col + row*128, tile byte at (z-1)/128 + *slot) but instead of a
 * passability bool it dispatches on the tile class g_tile_flags[tile]:
 *   6 -> find_grid_entity_facing_0xc0(x,y,z)   7 -> grid_hit_x(x,y,z)
 *   8 -> grid_hit_y(x,y,z)   9 -> find_grid_entity_facing_0x80(x,y,z)
 *   0xb -> try all four in that order, 1 on first hit
 *   0xa / anything else -> 0
 * Case map (table at 0x33e60, index = g_tile_flags[tile] - 6, ja > 5 -> default):
 *   idx0->0x33f08 idx1->0x33f17 idx2->0x33f26 idx3->0x33f35
 *   idx4(0xa)->0x33fa8(default) idx5(0xb)->0x33f44.
 * Ghidra mis-decodes the case heads: each true body starts one byte earlier with
 * PUSH EDI (0x57) at 0x33f08/0x33f17/0x33f26/0x33f35/0x33f44.
 */
extern char **volatile g_map_cols;
extern unsigned char *g_tile_flags;

int find_grid_entity_facing_0xc0(int x, int y, int z);
int grid_hit_x(int x, int y, int z);
int grid_hit_y(int x, int y, int z);
int find_grid_entity_facing_0x80(int x, int y, int z);

unsigned short map_tile_hit_dispatch(short x, short y, short z)
{
    unsigned short r;
    int xs;
    int row;
    int col;
    int index;
    unsigned char tile;

    int zz;
    xs = 0x6000;
    row = (y % xs) / 256;
    zz = z;
    xs = x;
    col = (xs & 0xff00) / 256;
    index = col + row * 128;
    tile = *(unsigned char *)((zz - 1) / 128 + (int)*(g_map_cols + index));
    switch (g_tile_flags[tile]) {
    case 6:
        return find_grid_entity_facing_0xc0(xs, y, z);
    case 7:
        return grid_hit_x(xs, y, z);
    case 8:
        return grid_hit_y(xs, y, z);
    case 9:
        return find_grid_entity_facing_0x80(xs, y, z);
    case 0xb:
        r = find_grid_entity_facing_0xc0(xs, y, z);
        if (r != 0)
            return 1;
        r = grid_hit_x(xs, y, z);
        if (r != 0)
            return 1;
        r = grid_hit_y(xs, y, z);
        if (r != 0)
            return 1;
        r = find_grid_entity_facing_0x80(xs, y, z);
        if (r != 0)
            return 1;
        return r;
    }
    return 0;
}
