/* prefix @ 0xf898 -- 4-corner passability test (previously-undecoded render-path prefix). For a tile
 * at world (param_1 hi-byte = tile-x, param_2 = y, param_3 = z/level), map to a grid cell and read the
 * passability flag via a triple indirection g_a510[g_tile_flags[ g_map_cols[row+col][z] ]]. Checks 4 corners
 * (y and x each +0x10); returns 1 if any is impassable (flag != 0), else 0. Only runs if param_3<0x600.
 * Signed divisions by powers of 2 (Watcom idiom). Framed (push ebx/esi/edi/ebp + 8B locals).
 * Recipe: -4s -oneatx -zp8 -s -zq.
 *
 * IMPROVED near-miss (was parked). `volatile short param_3` reproduces the target's key trait: the
 * original never caches param_3 in a register -- it compares it straight from memory
 * (`cmp word ptr [esp+0x24],0x600`) and re-reads it from the stack for the z=param_3/0x80 calc, leaving
 * ESI free for g_map_cols. Our non-volatile source cached param_3 in ESI (the whole-function
 * register-allocation cascade the old header described). With volatile the prologue, the param_3 guard,
 * and the entire first divide/row/col block are now byte-exact (first diff 0x7 -> 0x10, which is only a
 * layout-driven jge displacement; regdiff struct 76.4% -> 80.0%). Behaviour is identical: param_3 is a
 * by-value param nothing writes, so N volatile reads yield the same value.
 *
 * Residual (still unmatched, ours 371B vs target 378B): a stack-slot transpose ([esp]<->[esp+4] for the
 * row vs sign-extended-param_1 spills) coupled with an instruction-scheduling divergence -- the target
 * builds the cell pointer (lea edi,[g_map_cols + (row+col)*4]) BEFORE computing z, whereas our 9.5b
 * computes z first. An explicit `int px=param_1;` local flips the slot transpose but adds an early
 * reload that drops struct to 72%, so it is not kept. The slot+schedule pair is one coupled
 * allocator-cost-model cascade (same wall class as the corpus); decoded and much closer, not matched. */
extern unsigned char **g_map_cols;
extern unsigned char *g_tile_flags;
extern char g_a510[];

int passability_4corner(short param_1, short param_2, volatile short param_3)
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
