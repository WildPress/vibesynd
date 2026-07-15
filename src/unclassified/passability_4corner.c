/* prefix @ 0xf898 -- 4-corner passability test (previously-undecoded render-path prefix). For a tile
 * at world (param_1 hi-byte = tile-x, param_2 = y, param_3 = z/level), map to a grid cell and read the
 * passability flag via a triple indirection g_a510[g_tile_flags[ g_map_cols[row+col][z] ]]. Checks 4 corners
 * (y and x each +0x10); returns 1 if any is impassable (flag != 0), else 0. Only runs if param_3<0x600.
 * Signed divisions by powers of 2 (Watcom idiom). Framed (push ebx/esi/edi/ebp + 8B locals).
 * Recipe: -4s -oneatx -zp8 -s -zq.
 * PARKED near-miss: logic byte-correct, but Watcom caches param_3 in ESI (we compute z early) where
 * the target keeps g_map_cols in ESI and re-reads param_3 from memory -- a register-allocation cascade
 * (44 lines shift) that source order can't force. Same wall class as the corpus; decoded, not matched. */
extern unsigned char **g_map_cols;
extern unsigned char *g_tile_flags;
extern char g_a510[];

int passability_4corner(short param_1, short param_2, short param_3)
{
    if (param_3 < 0x600) {
        int row = (param_2 % 0x6000) / 0x100 * 0x80;
        int col = ((int)param_1 & 0xff00) / 0x100;
        int z = param_3 / 0x80;
        int row2;
        if (g_a510[g_tile_flags[g_map_cols[row + col][z]]] != 0)
            return 1;
        row2 = ((param_2 + 0x10) % 0x6000) / 0x100 * 0x80;
        if (g_a510[g_tile_flags[g_map_cols[row2 + col][z]]] != 0)
            return 1;
        col = (((int)param_1 + 0x10) & 0xff00) / 0x100;
        if (g_a510[g_tile_flags[g_map_cols[row + col][z]]] != 0)
            return 1;
        if (g_a510[g_tile_flags[g_map_cols[col + row2][z]]] != 0)
            return 1;
    }
    return 0;
}
