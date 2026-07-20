/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): switch binary-search tree
   balance. Audited whole function -- +0x18 gate, +0x19 type read, stride 0x2a,
   loop bound, and all eight +0x14 store constants (0x258/0x64/0x50/0x1e/0x28/
   0x0a/0x78/0x73) are byte-identical; the same 16-value case set routes to the
   same stores. Only the binary-search pivot comparisons and jb/jbe tree shape
   differ (Watcom pivots 4/4 at 0x0f, ours 3/5 at 0x0d). Output-equivalent.
 *
 * frameless @ 0x20d98: PARKED near-miss (switch-tree balance). Logic, register
   (EDX cursor), loop bounds and all eight cases are correct; the only diff is
   how Watcom balances the switch's binary-search tree (it pivots the case-ranges
   4/4 at 0x0f, ours 3/5 at 0x0d), which we can't steer from C. Kept for the
   game data it documents.

   For every in-use record in pool B ([0xdd10,0xe790), stride 0x2a), set the word
   at +0x14 from the record's type byte at +0x19. This is per-type stat init: the
   values look like hit-points, and the types come in consecutive pairs (probably
   two orientations/variants of the same object). Type -> value:
     0x01,0x02 -> 600   0x05,0x06 -> 100   0x09,0x0a -> 80    0x0d,0x0e -> 30
     0x11,0x12 -> 40    0x1c,0x1d -> 10    0x24,0x25 -> 120   0x28,0x29 -> 115
   everything else is left untouched. */
extern unsigned char pool_b[];   /* 0xdd10 */
extern unsigned char pool_c[];   /* 0xe790 (end of pool B) */
void vehicle_hp_stamp(void)
{
    unsigned char *p;
    for (p = pool_b; p < pool_c; p += 0x2a) {
        if (p[0x18] == 0)
            continue;
        switch (p[0x19]) {
        case 0x01: case 0x02: *(unsigned short *)(p + 0x14) = 0x258; break;
        case 0x05: case 0x06: *(unsigned short *)(p + 0x14) = 0x64;  break;
        case 0x09: case 0x0a: *(unsigned short *)(p + 0x14) = 0x50;  break;
        case 0x0d: case 0x0e: *(unsigned short *)(p + 0x14) = 0x1e;  break;
        case 0x11: case 0x12: *(unsigned short *)(p + 0x14) = 0x28;  break;
        case 0x1c: case 0x1d: *(unsigned short *)(p + 0x14) = 0x0a;  break;
        case 0x24: case 0x25: *(unsigned short *)(p + 0x14) = 0x78;  break;
        case 0x28: case 0x29: *(unsigned short *)(p + 0x14) = 0x73;  break;
        }
    }
}
