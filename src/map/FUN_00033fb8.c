/* MATCHED 137/137 (reloc-aware) -- frameless map-passability check @ 0x33fb8.
 * Levers that closed the old g_map_cols register wall (cont. 22): pointer-variable
 * decl `char **g_map_cols`; `base = g_map_cols;` declared BETWEEN col and index (the
 * mid-computation live-range start is load-bearing twice over: right before
 * slot it copy-propagates back into the coalesced `add ebx,eax` form, at the
 * top of the fn it compiles but homes base in ECX; between col and index it
 * gives the target's `mov edx,[g_map_cols]` + `lea ebx,[edx+eax]` with the movsx
 * of z AFTER the lea); slot local + divide-first `(int)*slot` deref
 * (`add eax,[ebx]`); and a SWITCH on the tile type (cases 6-9,0xb) -- the
 * &&/|| chain emits `jb +4` (still tests 0xb when <6), switch lowering gives
 * the target's `jb +0f` straight to return 0.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char **g_map_cols;
extern unsigned char *g_tile_flags;
int FUN_00033fb8(short x, short y, short z)
{
    int row = (y % 0x6000) / 256;
    int col = (x & 0xff00) / 256;
    char **base = g_map_cols;
    int index = col + row * 128;
    char **slot = base + index;
    unsigned char tile;
    tile = *(unsigned char *)(z / 128 + (int)*slot);
    switch (g_tile_flags[tile]) {
    case 6:
    case 7:
    case 8:
    case 9:
    case 0xb:
        return 1;
    }
    return 0;
}
