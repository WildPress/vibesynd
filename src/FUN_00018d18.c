/* FUN_00018d18 (1523B) -- inline jump-table switch dispatcher. NEAR-MISS / WALL.
 *
 * STRUCTURE (fully reverse-engineered from the disasm; jump-table verification is
 * now available and the leading [table][pad][code] layout is detected correctly):
 *   clamp n to [2,8]; switch(n) via `jmp CS:[EBX*4+0xb5ac]` over 7 dense cases.
 *   Each case draws a symmetric FILLED shape row-by-row: a top run of n rows plus a
 *   bottom run of n-1 rows (2n-1 total), one FUN_0003f636(lo,hi,row,d) call per row.
 *   lo=X-h, hi=X+h are symmetric about X; the bottom rows reuse (reload) the mirrored
 *   top-row lo/hi (hence Watcom's store-to-slot / reload-from-slot spill pattern).
 *   The per-row half-widths h (top rows, i=0..n-1) decoded exactly are:
 *     n=2:0,1  n=3:1,2,2  n=4:1,2,3,3  n=5:1,3,3,4,4  n=6:2,4,4,5,5,5
 *     n=7:2,4,5,5,6,6,6   n=8:2,4,5,6,6,7,7,7
 *   (Ghidra's decompile/disasm mislabels each case's entry -- it skips the per-case
 *   `movzx d / movsx Y / movsx X` preamble -- so the older "n unrolled calls" note was
 *   wrong; it is 2n-1 calls forming a filled hexagon-ish tile blob.)
 *
 * WHY IT DOESN'T CLOSE (compound §3 walls, not source-reachable):
 *   1) Register-role wall: the target allocates px->EAX, py->EDI, n->EBX, d->EDX;
 *      Watcom 9.5b puts py->ESI here (d then takes EDI), swapping ESI<->EDI in EVERY
 *      case (movzx esi,dl vs edi, etc.). Tried scalar/array locals, decl order,
 *      compute order (X-first / Y-first), a function-scope `int dd=d` copy, and
 *      recipes -oneatx/-or/-ot/-os/-ol -- none reproduce (px,py,n,d)=(eax,edi,ebx,edx).
 *   2) Frame/scheduling: this explicit unroll needs more temporaries than the target's
 *      per-case unrolled LOOP, giving `sub esp,0x70` vs the target's `0x64` and ~108B
 *      of extra code (1631 vs 1523). The true source is a per-case loop whose closed-
 *      form half-width formula is NOT recoverable from the (irregular) width values
 *      (no polynomial / sqrt / floor-division fits all 7 cases), so the loop that would
 *      reproduce Watcom's exact unroller schedule + spill slots can't be written.
 *
 * The n=2 case IS byte-structurally identical to the target modulo the ESI/EDI swap
 * (63B vs 62B), confirming the reconstruction is correct; the walls above block a
 * masked byte match. Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern void FUN_0003f636(int lo, int hi, int row, int d);
void FUN_00018d18(int px, int py, short n, unsigned char d)
{
    if (n <= 1) n = 2;
    else if (n > 8) n = 8;
    switch (n) {
    case 2:
        {
            int sx, sy, l0, r0, l1, r1;
            sx = (short)px; sy = (short)py;
            l0 = sx - 0; r0 = sx + 0; FUN_0003f636(l0, r0, sy - 1, d);
            l1 = sx - 1; r1 = sx + 1; FUN_0003f636(l1, r1, sy, d);
            FUN_0003f636(l0, r0, sy + 1, d);
        }
        break;
    case 3:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; FUN_0003f636(l0, r0, sy - 2, d);
            l1 = sx - 2; r1 = sx + 2; FUN_0003f636(l1, r1, sy - 1, d);
            l2 = sx - 2; r2 = sx + 2; FUN_0003f636(l2, r2, sy, d);
            FUN_0003f636(l1, r1, sy + 1, d);
            FUN_0003f636(l0, r0, sy + 2, d);
        }
        break;
    case 4:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; FUN_0003f636(l0, r0, sy - 3, d);
            l1 = sx - 2; r1 = sx + 2; FUN_0003f636(l1, r1, sy - 2, d);
            l2 = sx - 3; r2 = sx + 3; FUN_0003f636(l2, r2, sy - 1, d);
            l3 = sx - 3; r3 = sx + 3; FUN_0003f636(l3, r3, sy, d);
            FUN_0003f636(l2, r2, sy + 1, d);
            FUN_0003f636(l1, r1, sy + 2, d);
            FUN_0003f636(l0, r0, sy + 3, d);
        }
        break;
    case 5:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; FUN_0003f636(l0, r0, sy - 4, d);
            l1 = sx - 3; r1 = sx + 3; FUN_0003f636(l1, r1, sy - 3, d);
            l2 = sx - 3; r2 = sx + 3; FUN_0003f636(l2, r2, sy - 2, d);
            l3 = sx - 4; r3 = sx + 4; FUN_0003f636(l3, r3, sy - 1, d);
            l4 = sx - 4; r4 = sx + 4; FUN_0003f636(l4, r4, sy, d);
            FUN_0003f636(l3, r3, sy + 1, d);
            FUN_0003f636(l2, r2, sy + 2, d);
            FUN_0003f636(l1, r1, sy + 3, d);
            FUN_0003f636(l0, r0, sy + 4, d);
        }
        break;
    case 6:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; FUN_0003f636(l0, r0, sy - 5, d);
            l1 = sx - 4; r1 = sx + 4; FUN_0003f636(l1, r1, sy - 4, d);
            l2 = sx - 4; r2 = sx + 4; FUN_0003f636(l2, r2, sy - 3, d);
            l3 = sx - 5; r3 = sx + 5; FUN_0003f636(l3, r3, sy - 2, d);
            l4 = sx - 5; r4 = sx + 5; FUN_0003f636(l4, r4, sy - 1, d);
            l5 = sx - 5; r5 = sx + 5; FUN_0003f636(l5, r5, sy, d);
            FUN_0003f636(l4, r4, sy + 1, d);
            FUN_0003f636(l3, r3, sy + 2, d);
            FUN_0003f636(l2, r2, sy + 3, d);
            FUN_0003f636(l1, r1, sy + 4, d);
            FUN_0003f636(l0, r0, sy + 5, d);
        }
        break;
    case 7:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5, l6, r6;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; FUN_0003f636(l0, r0, sy - 6, d);
            l1 = sx - 4; r1 = sx + 4; FUN_0003f636(l1, r1, sy - 5, d);
            l2 = sx - 5; r2 = sx + 5; FUN_0003f636(l2, r2, sy - 4, d);
            l3 = sx - 5; r3 = sx + 5; FUN_0003f636(l3, r3, sy - 3, d);
            l4 = sx - 6; r4 = sx + 6; FUN_0003f636(l4, r4, sy - 2, d);
            l5 = sx - 6; r5 = sx + 6; FUN_0003f636(l5, r5, sy - 1, d);
            l6 = sx - 6; r6 = sx + 6; FUN_0003f636(l6, r6, sy, d);
            FUN_0003f636(l5, r5, sy + 1, d);
            FUN_0003f636(l4, r4, sy + 2, d);
            FUN_0003f636(l3, r3, sy + 3, d);
            FUN_0003f636(l2, r2, sy + 4, d);
            FUN_0003f636(l1, r1, sy + 5, d);
            FUN_0003f636(l0, r0, sy + 6, d);
        }
        break;
    case 8:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5, l6, r6, l7, r7;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; FUN_0003f636(l0, r0, sy - 7, d);
            l1 = sx - 4; r1 = sx + 4; FUN_0003f636(l1, r1, sy - 6, d);
            l2 = sx - 5; r2 = sx + 5; FUN_0003f636(l2, r2, sy - 5, d);
            l3 = sx - 6; r3 = sx + 6; FUN_0003f636(l3, r3, sy - 4, d);
            l4 = sx - 6; r4 = sx + 6; FUN_0003f636(l4, r4, sy - 3, d);
            l5 = sx - 7; r5 = sx + 7; FUN_0003f636(l5, r5, sy - 2, d);
            l6 = sx - 7; r6 = sx + 7; FUN_0003f636(l6, r6, sy - 1, d);
            l7 = sx - 7; r7 = sx + 7; FUN_0003f636(l7, r7, sy, d);
            FUN_0003f636(l6, r6, sy + 1, d);
            FUN_0003f636(l5, r5, sy + 2, d);
            FUN_0003f636(l4, r4, sy + 3, d);
            FUN_0003f636(l3, r3, sy + 4, d);
            FUN_0003f636(l2, r2, sy + 5, d);
            FUN_0003f636(l1, r1, sy + 6, d);
            FUN_0003f636(l0, r0, sy + 7, d);
        }
        break;
    }
}
