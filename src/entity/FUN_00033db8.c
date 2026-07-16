/* frameless @ 0x33db8: grid query -- is there a matching entity in the 6 grid rows
   at/below (param0.x, y0)? For each of 6 rows (y stepped by 0x100 from y0-0x100),
   walk the spatial-grid cell's entity chain (g_grid_heads[cell] -> g_entity_pool[id],
   linked via word0). Return 1 on the first entity that is state 2, shares the high byte
   of field+8 with `match`, has a non-zero link (+0x1c) and flag (+0x1a)!=0x80, and whose
   linked entity's +0x54 is non-zero. Return 0 if none.

   PARKED near-miss. `unsigned short row` fixes the counter (16-bit `cmp di,6; jb`, edi/esi
   roles), so the loop body + tail match. Remaining diffs are top-of-function scheduling and
   index-register allocation, not source-reachable (cpermute 3000 variants, no match): the
   original loads y0/match in the opposite order and schedules `sub esi`/`xor edi` the other
   way, and keeps the grid cell in edx with scaled `[edx*2+heads]` + `cwde`, where we
   pre-double into eax with `movsx`. Scheduling/register-role family. */
extern unsigned short g_grid_heads[];
extern unsigned char g_entity_pool[];

int FUN_00033db8(short param0, int y0, int match)
{
    unsigned short row;
    int y = y0 - 0x100;
    for (row = 0; row < 6; row++) {
        int ym = (short)(y & 0x7f00);
        int cell = (ym >> 1) | (((int)param0 >> 8) & 0x7f);
        unsigned id = g_grid_heads[cell];
        while (id != 0) {
            unsigned char *rec = g_entity_pool + id;
            if (rec[0x18] == 2
                && (*(short *)(rec + 8) & 0xff00) == ((short)match & 0xff00)
                && *(unsigned short *)(rec + 0x1c) != 0
                && rec[0x1a] != 0x80) {
                unsigned char *rec2 = g_entity_pool + *(unsigned short *)(rec + 0x1c);
                if (rec2[0x54] != 0)
                    return 1;
            }
            id = *(unsigned short *)rec;
        }
        y += 0x100;
    }
    return 0;
}
