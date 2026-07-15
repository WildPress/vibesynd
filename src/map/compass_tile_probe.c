/* @ 0x34368 -- 4-way compass neighbour tile-type probe.  dir (arg3, byte) selects
   one of N/E/S/W (0/0x40/0x80/0xc0, else return 0); each direction offsets the
   moving coordinate by the direction-vector table (g_dir_dy dy for N/S, g_dir_dx dx
   for E/W), computes the map column index (row = ((off+y)%0x6000)/0x100 <<7,
   col = ((off+x)&0xff00)/0x100) into the g_map_cols column-pointer table, reads the
   tile one level down ((z-1)/0x80 + column base), and returns 1 iff its type byte
   g_tile_flags[tile] equals the per-direction expected type (N=8, E=7, S=9, W=6),
   else 0.  Signed div idioms: %0x6000 (idiv), /0x100 and /0x80 (sar-with-sbb).
   g_map_cols/g_tile_flags pointer vars saved in ESI/EDI and re-stored at every return
   (single-exit).  Sibling idioms: FUN_00033fb8 / FUN_00028ec8 (g_map_cols column).
   Recipe: -4s -oneatx -zp8 -s -zq

   PARKED near-miss: ours 659B vs target 666B, difflib 0.334. The instruction
   STREAM is byte-faithful -- same switch binary-tree dispatch, same signed
   idioms (movsx g_dir_dy[dir]; shl 8; sar 8; add; idiv 0x6000; the sar-0x1f/shl/
   sbb/sar signed /0x100 and /0x80), same idx=row<<7+col column lookup, same
   `<<8>>8` on the direction value (NOT optimized away), same single-exit store-
   backs. The ENTIRE diff is one register-allocation tie that cascades through
   every modrm byte: the target keeps `x` MEMORY-homed and enregisters
   {base->ESI, tt->EDI, y->ECX}; ours PROMOTES `x` into callee-saved EDI at entry
   and demotes the globals to {base->EBX, tt->ECX}, flipping the 0x6000 divisor
   (EBX->ESI) and the g_dir_dy temp (EDX->EAX) everywhere. This is the section-3
   param-auto-promotion wall (AGENTS flagged "poor exact-match shape"): a
   `short yy=y` register-resident copy (cont.21 lever) did NOT suppress the x
   promotion nor move the entry-load order (first diff stayed at 0x4). Not
   source-reachable. */
extern char **g_map_cols;
extern unsigned char *g_tile_flags;
extern short g_dir_dx[];
extern short g_dir_dy[];

int compass_tile_probe(short x, short y, short z, unsigned char dir)
{
    char **base = g_map_cols;
    unsigned char *tt = g_tile_flags;
    int r;
    int row, col, idx;
    unsigned char tile;

    switch (dir) {
    case 0:
        row = ((g_dir_dy[0] << 8 >> 8) + y) % 0x6000 / 0x100;
        col = (x & 0xff00) / 0x100;
        idx = row << 7;
        idx += col;
        tile = *(unsigned char *)((z - 1) / 0x80 + (int)base[idx]);
        if (tt[tile] == 8) { r = 1; goto done; }
        break;
    case 0x40:
        col = (((g_dir_dx[0x40] << 8 >> 8) + x) & 0xff00) / 0x100;
        row = y % 0x6000 / 0x100;
        idx = col;
        idx += row << 7;
        tile = *(unsigned char *)((z - 1) / 0x80 + (int)base[idx]);
        if (tt[tile] == 7) { r = 1; goto done; }
        break;
    case 0x80:
        row = ((g_dir_dy[0x80] << 8 >> 8) + y) % 0x6000 / 0x100;
        col = (x & 0xff00) / 0x100;
        idx = row << 7;
        idx += col;
        tile = *(unsigned char *)((z - 1) / 0x80 + (int)base[idx]);
        if (tt[tile] == 9) { r = 1; goto done; }
        break;
    case 0xc0:
        col = (((g_dir_dx[0xc0] << 8 >> 8) + x) & 0xff00) / 0x100;
        row = y % 0x6000 / 0x100;
        idx = col;
        idx += row << 7;
        tile = *(unsigned char *)((z - 1) / 0x80 + (int)base[idx]);
        if (tt[tile] == 6) { r = 1; goto done; }
        break;
    }
    r = 0;
done:
    g_tile_flags = tt;
    g_map_cols = base;
    return r;
}
