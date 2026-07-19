/* draw_filled_shape (1523B) -- unrolled jump-table switch dispatcher.
 * Best edit-dist 697 (was 732); match 57.6% (was 56.0%). Residual = codegen wall.
 *
 * STRUCTURE (fully reverse-engineered from the disasm):
 *   clamp n to [2,8]; switch(n) via `jmp CS:[EBX*4+table]` over 7 dense cases.
 *   Each case draws a symmetric FILLED blob row-by-row: a top run of n rows plus a
 *   bottom run of n-1 rows (2n-1 total), one draw_hline(lo,hi,row,d) call per row.
 *   lo=X-h, hi=X+h are symmetric about X; the bottom rows reuse (reload) the mirrored
 *   top-row lo/hi (hence Watcom's store-to-slot / reload-from-slot spill pattern).
 *   Each case is FULLY UNROLLED in the target (no back-edge) -- confirmed by reading
 *   every case region; the earlier "per-case loop" guess was wrong.
 *   The per-row half-widths h (top rows, i=0..n-1) decoded exactly are:
 *     n=2:0,1  n=3:1,2,2  n=4:1,2,3,3  n=5:1,3,3,4,4  n=6:2,4,4,5,5,5
 *     n=7:2,4,5,5,6,6,6   n=8:2,4,5,6,6,7,7,7
 *
 * WHAT MOVED THE NEEDLE:
 *   Promoting the byte param d to a function-scope `int dv = d` (at the very top,
 *   before the clamp) homes d in a callee-saved reg (EBX) for the whole function and,
 *   as a side effect, makes py land in EDI in the per-case body -- matching the
 *   target's py->EDI in e.g. case 2. That single change dropped 732->697. Placement
 *   matters: dv before the clamp beats dv after it (697 vs 701). The per-case
 *   `sx=(short)px; sy=(short)py;` casts MUST stay inside each case -- hoisting them to
 *   function scope destroys the per-case movsx pattern (blows up to 1200-2000).
 *
 * REMAINING GAP (compound codegen ties, not source-reachable):
 *   1) Register roles: the target's allocation VARIES per case -- case 2 is
 *      px->EBX,d->ESI,py->EDI, but case 4 is px->EDI,d->EBX(xor+mov bl,dl),py->ESI.
 *      No single source spelling reproduces all 7 cases' colourings at once; the dv
 *      trick fixes py but leaves px<->d swapped vs case 2 (ours px->ESI / d->EBX).
 *   2) Frame: this explicit unroll still needs `sub esp,0x70` vs the target's `0x64`
 *      (3 extra spill slots); merging equal-width rows to shared temps does not shrink
 *      it (Watcom allocates the same frame either way, 712 -- kept the distinct-var
 *      form). ~120B of extra length remains.
 *   3) Layout: our build emits the switch jump-table INLINE at the front of the
 *      function ([table][pad][code]); the target keeps its table in a separate segment
 *      so its function is pure code from byte 0. First-diff sits at 0x1c inside that
 *      inline table -- a linker/codegen placement the source cannot move.
 * Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern void draw_hline(int lo, int hi, int row, int d);
void draw_filled_shape(int px, int py, short n, unsigned char d)
{
    int dv = d;
    if (n <= 1) n = 2;
    else if (n > 8) n = 8;
    switch (n) {
    case 2:
        {
            int sx, sy, l0, r0, l1, r1;
            sx = (short)px; sy = (short)py;
            l0 = sx - 0; r0 = sx + 0; draw_hline(l0, r0, sy - 1, dv);
            l1 = sx - 1; r1 = sx + 1; draw_hline(l1, r1, sy, dv);
            draw_hline(l0, r0, sy + 1, dv);
        }
        break;
    case 3:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; draw_hline(l0, r0, sy - 2, dv);
            l1 = sx - 2; r1 = sx + 2; draw_hline(l1, r1, sy - 1, dv);
            l2 = sx - 2; r2 = sx + 2; draw_hline(l2, r2, sy, dv);
            draw_hline(l1, r1, sy + 1, dv);
            draw_hline(l0, r0, sy + 2, dv);
        }
        break;
    case 4:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; draw_hline(l0, r0, sy - 3, dv);
            l1 = sx - 2; r1 = sx + 2; draw_hline(l1, r1, sy - 2, dv);
            l2 = sx - 3; r2 = sx + 3; draw_hline(l2, r2, sy - 1, dv);
            l3 = sx - 3; r3 = sx + 3; draw_hline(l3, r3, sy, dv);
            draw_hline(l2, r2, sy + 1, dv);
            draw_hline(l1, r1, sy + 2, dv);
            draw_hline(l0, r0, sy + 3, dv);
        }
        break;
    case 5:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4;
            sx = (short)px; sy = (short)py;
            l0 = sx - 1; r0 = sx + 1; draw_hline(l0, r0, sy - 4, dv);
            l1 = sx - 3; r1 = sx + 3; draw_hline(l1, r1, sy - 3, dv);
            l2 = sx - 3; r2 = sx + 3; draw_hline(l2, r2, sy - 2, dv);
            l3 = sx - 4; r3 = sx + 4; draw_hline(l3, r3, sy - 1, dv);
            l4 = sx - 4; r4 = sx + 4; draw_hline(l4, r4, sy, dv);
            draw_hline(l3, r3, sy + 1, dv);
            draw_hline(l2, r2, sy + 2, dv);
            draw_hline(l1, r1, sy + 3, dv);
            draw_hline(l0, r0, sy + 4, dv);
        }
        break;
    case 6:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; draw_hline(l0, r0, sy - 5, dv);
            l1 = sx - 4; r1 = sx + 4; draw_hline(l1, r1, sy - 4, dv);
            l2 = sx - 4; r2 = sx + 4; draw_hline(l2, r2, sy - 3, dv);
            l3 = sx - 5; r3 = sx + 5; draw_hline(l3, r3, sy - 2, dv);
            l4 = sx - 5; r4 = sx + 5; draw_hline(l4, r4, sy - 1, dv);
            l5 = sx - 5; r5 = sx + 5; draw_hline(l5, r5, sy, dv);
            draw_hline(l4, r4, sy + 1, dv);
            draw_hline(l3, r3, sy + 2, dv);
            draw_hline(l2, r2, sy + 3, dv);
            draw_hline(l1, r1, sy + 4, dv);
            draw_hline(l0, r0, sy + 5, dv);
        }
        break;
    case 7:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5, l6, r6;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; draw_hline(l0, r0, sy - 6, dv);
            l1 = sx - 4; r1 = sx + 4; draw_hline(l1, r1, sy - 5, dv);
            l2 = sx - 5; r2 = sx + 5; draw_hline(l2, r2, sy - 4, dv);
            l3 = sx - 5; r3 = sx + 5; draw_hline(l3, r3, sy - 3, dv);
            l4 = sx - 6; r4 = sx + 6; draw_hline(l4, r4, sy - 2, dv);
            l5 = sx - 6; r5 = sx + 6; draw_hline(l5, r5, sy - 1, dv);
            l6 = sx - 6; r6 = sx + 6; draw_hline(l6, r6, sy, dv);
            draw_hline(l5, r5, sy + 1, dv);
            draw_hline(l4, r4, sy + 2, dv);
            draw_hline(l3, r3, sy + 3, dv);
            draw_hline(l2, r2, sy + 4, dv);
            draw_hline(l1, r1, sy + 5, dv);
            draw_hline(l0, r0, sy + 6, dv);
        }
        break;
    case 8:
        {
            int sx, sy, l0, r0, l1, r1, l2, r2, l3, r3, l4, r4, l5, r5, l6, r6, l7, r7;
            sx = (short)px; sy = (short)py;
            l0 = sx - 2; r0 = sx + 2; draw_hline(l0, r0, sy - 7, dv);
            l1 = sx - 4; r1 = sx + 4; draw_hline(l1, r1, sy - 6, dv);
            l2 = sx - 5; r2 = sx + 5; draw_hline(l2, r2, sy - 5, dv);
            l3 = sx - 6; r3 = sx + 6; draw_hline(l3, r3, sy - 4, dv);
            l4 = sx - 6; r4 = sx + 6; draw_hline(l4, r4, sy - 3, dv);
            l5 = sx - 7; r5 = sx + 7; draw_hline(l5, r5, sy - 2, dv);
            l6 = sx - 7; r6 = sx + 7; draw_hline(l6, r6, sy - 1, dv);
            l7 = sx - 7; r7 = sx + 7; draw_hline(l7, r7, sy, dv);
            draw_hline(l6, r6, sy + 1, dv);
            draw_hline(l5, r5, sy + 2, dv);
            draw_hline(l4, r4, sy + 3, dv);
            draw_hline(l3, r3, sy + 4, dv);
            draw_hline(l2, r2, sy + 5, dv);
            draw_hline(l1, r1, sy + 6, dv);
            draw_hline(l0, r0, sy + 7, dv);
        }
        break;
    }
}
