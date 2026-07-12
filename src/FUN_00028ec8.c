/* NEAR-MISS @ 0x28ec8 -- ~200/218; PARKED on the cache-vs-fold wall (0x269d8
 * class). Target RE-READS the column pointer per test (add eax,[ecx] twice,
 * entry addr cached in ECX); -oneatx CSE-merges the load once into a register
 * (mov ecx,[ecx]; cmp [ecx+eax]) and no spelling splits it: inline exprs,
 * named pointer temp (copy-propagated away), char * volatile * (volatile via
 * pointer IGNORED by 9.5 CSE), operand commutes, named idx. Lighter -or fixes
 * the movsx-z placement but keeps the merged load + accumulate direction
 * (add eax,ecx vs add ecx,eax). Semantics fully decoded below.
 *
   0x28ec8 -- vertical probe + drift clamp. Computes the map column for the
 * object's tile (g_5358[((y % 0x6000)/0x100)*0x80 + (x & 0xff00)/0x100], the
 * 0x33fb8 passability idiom); if the tile byte one level down ((z-0x80)/0x80)
 * or two down ((z-0x100)/0x80) is 0x76 the object rises (z += 0x10), else it
 * falls (z -= 8). Then x and y each step toward alignment via the clamp
 * 0x34048(coord, 0x80), and if z went negative 0x269b8(b) (remove?).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char **g_5358;
extern short FUN_00034048(int cur, int step);
extern void FUN_000269b8(unsigned char *b);

void FUN_00028ec8(unsigned char *b)
{
    int idx = (*(short *)(b + 6) % 0x6000 / 0x100 << 7)
            + (*(short *)(b + 4) & 0xff00) / 0x100;
    int z = *(short *)(b + 8);
    char *t;

    t = (z - 0x80) / 0x80 + g_5358[idx];
    if (*t == 0x76) {
        *(short *)(b + 8) += 0x10;
    } else {
        t = (z - 0x100) / 0x80 + g_5358[idx];
        if (*t == 0x76)
            *(short *)(b + 8) += 0x10;
        else
            *(short *)(b + 8) -= 8;
    }
    *(short *)(b + 4) = FUN_00034048(*(short *)(b + 4), 0x80);
    *(short *)(b + 6) = FUN_00034048(*(short *)(b + 6), 0x80);
    if (*(short *)(b + 8) < 0)
        FUN_000269b8(b);
}
