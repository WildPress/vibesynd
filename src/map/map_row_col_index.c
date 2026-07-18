/* MATCHED 218/218 (reloc-aware) -- the cont. 22 retry closed the old
 * cache-vs-fold wall in ONE compile with the 0x2fca8/0x33fb8 lever set:
 * named int accumulator (`idx = row<<7; idx += col;` -- homes idx in ECX and
 * gives the target's `add ecx,eax` accumulate direction, where the one-expr
 * form homed idx in EAX); `base = g_map_cols;` named base (base lands in the
 * freed EAX = the a1 moffs load); `slot = base + idx;`; and divide-first
 * `(int)*slot` derefs in each test, which keep the column pointer as a folded
 * memory operand (`add eax,[ecx]` per test) instead of the CSE-merged
 * `mov ecx,[ecx]; cmp [ecx+eax]`. The movsx of z scheduled itself between the
 * shl and the accumulate exactly as in the target.
 *
   0x28ec8 -- vertical probe + drift clamp. Computes the map column for the
 * object's tile (g_map_cols[((y % 0x6000)/0x100)*0x80 + (x & 0xff00)/0x100], the
 * 0x33fb8 passability idiom); if the tile byte one level down ((z-0x80)/0x80)
 * or two down ((z-0x100)/0x80) is 0x76 the object rises (z += 0x10), else it
 * falls (z -= 8). Then x and y each step toward alignment via the clamp
 * 0x34048(coord, 0x80), and if z went negative 0x269b8(b) (remove?).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char **g_map_cols;
extern short snap_direction(int cur, int step);
extern void FUN_000269b8(unsigned char *b);

void map_row_col_index(unsigned char *b)
{
    char **base;
    char **slot;
    int idx;
    int z = *(short *)(b + 8);

    idx = *(short *)(b + 6) % 0x6000 / 0x100 << 7;
    idx += (*(short *)(b + 4) & 0xff00) / 0x100;
    base = g_map_cols;
    slot = base + idx;
    if (*(char *)((z - 0x80) / 0x80 + (int)*slot) == 0x76) {
        *(short *)(b + 8) += 0x10;
    } else if (*(char *)((z - 0x100) / 0x80 + (int)*slot) == 0x76) {
        *(short *)(b + 8) += 0x10;
    } else {
        *(short *)(b + 8) -= 8;
    }
    *(short *)(b + 4) = snap_direction(*(short *)(b + 4), 0x80);
    *(short *)(b + 6) = snap_direction(*(short *)(b + 6), 0x80);
    if (*(short *)(b + 8) < 0)
        FUN_000269b8(b);
}
