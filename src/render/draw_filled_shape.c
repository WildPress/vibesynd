/* draw_filled_shape (1523B) -- unrolled jump-table switch dispatcher.
 * edit-dist 516; match 66.1%; ourcode 1521B (len_delta -2). (Measured with
 * tools/_eco_measure.py: masked, jump-table-aware Levenshtein vs the target body.)
 *
 * STRUCTURE (fully reverse-engineered from the disasm):
 *   clamp n to [2,8]; switch(n) via `jmp CS:[reg*4+table]` over 7 dense cases.
 *   Each case draws a symmetric FILLED blob row-by-row: a top run of n rows plus a
 *   bottom run of n-1 rows (2n-1 total), one draw_hline(lo,hi,row,d) call per row.
 *   lo=X-h, hi=X+h are symmetric about X; the bottom rows reuse the mirrored top-row
 *   lo/hi. The target computes each top-row lo/hi into EAX, PUSHES it directly, and
 *   caches a copy to a stack slot (`push eax; mov [esp+k],eax`); the mirror bottom row
 *   RELOADS that slot. Each case is FULLY UNROLLED (no back-edge).
 *   The per-row half-widths h (top rows, i=0..n-1) decoded exactly are:
 *     n=2:0,1  n=3:1,2,2  n=4:1,2,3,3  n=5:1,3,3,4,4  n=6:2,4,4,5,5,5
 *     n=7:2,4,5,5,6,6,6   n=8:2,4,5,6,6,7,7,7
 *
 * WHAT MOVED THE NEEDLE (821->516 edit-dist, +70B->-2B length):
 *   1) INLINE the lo/hi expressions straight into each draw_hline call -- do NOT
 *      materialise `lN=sx-h; rN=sx+h;` temporaries. The old distinct-variable form made
 *      Watcom store both operands to their home slots and then RELOAD them for the very
 *      next call (redundant store+reload per top row) and inflated the frame to
 *      sub esp,0x70. Inlining lets Watcom CSE the reused sx+-h, push the fresh EAX
 *      directly, and cache-for-mirror -- exactly the target's shape -- shrinking the
 *      frame to sub esp,0x58 and dropping length from +70B to -2B.
 *   2) Pass the byte param `d` DIRECTLY (no `int dv=d`). In this inlined structure the
 *      dv pin costs (516 without dv vs 594 with): d wants to live in DL/DX at its home
 *      and be reloaded per case as `xor ebx,ebx; mov bl,dl` -- matching the target's
 *      per-case d-load -- rather than being pinned in a callee-saved reg all function.
 *   The per-case `sx=(short)px; sy=(short)py;` casts MUST stay inside each case --
 *   hoisting them to function scope destroys the per-case movsx pattern.
 *
 * REMAINING GAP (codegen ties, not source-reachable):
 *   1) Param register colouring: the target holds n in EBX (switch index) and py in EDI;
 *      ours holds n in (E)SI and py in (E)SI/ESI. Both then movsx into the same sx->EDI,
 *      sy->ESI per-case roles, so the divergence is a 1-byte modrm (di vs si) repeated
 *      across every case plus the switch-index reg. No source spelling flips Watcom's
 *      linear-scan colouring of the two int params (confirmed: decl order / assign order
 *      / cast form all leave edit-dist at 516).
 *   2) Frame slot offsets: ours assigns the cache slots at different [esp+k] than the
 *      target (0x58 vs 0x64 frame), so the cache/reload disp8 bytes differ.
 *   3) Layout: our single-fn build emits the switch jump-table INLINE at the front of
 *      the object ([table][pad][code]); the shipped binary keeps its table in a separate
 *      CS segment (pure code from byte 0). First-diff sits at 0x1c inside that inline
 *      table -- a linker/codegen placement the source cannot move (build-isolation
 *      artefact, same class as parse_hex_arg's).
 * Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern void draw_hline(int lo, int hi, int row, int d);
void draw_filled_shape(int px, int py, short n, unsigned char d)
{
    if (n <= 1) n = 2;
    else if (n > 8) n = 8;
    switch (n) {
    case 2:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 0, sx + 0, sy - 1, d);
            draw_hline(sx - 1, sx + 1, sy, d);
            draw_hline(sx - 0, sx + 0, sy + 1, d);
        }
        break;
    case 3:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 1, sx + 1, sy - 2, d);
            draw_hline(sx - 2, sx + 2, sy - 1, d);
            draw_hline(sx - 2, sx + 2, sy, d);
            draw_hline(sx - 2, sx + 2, sy + 1, d);
            draw_hline(sx - 1, sx + 1, sy + 2, d);
        }
        break;
    case 4:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 1, sx + 1, sy - 3, d);
            draw_hline(sx - 2, sx + 2, sy - 2, d);
            draw_hline(sx - 3, sx + 3, sy - 1, d);
            draw_hline(sx - 3, sx + 3, sy, d);
            draw_hline(sx - 3, sx + 3, sy + 1, d);
            draw_hline(sx - 2, sx + 2, sy + 2, d);
            draw_hline(sx - 1, sx + 1, sy + 3, d);
        }
        break;
    case 5:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 1, sx + 1, sy - 4, d);
            draw_hline(sx - 3, sx + 3, sy - 3, d);
            draw_hline(sx - 3, sx + 3, sy - 2, d);
            draw_hline(sx - 4, sx + 4, sy - 1, d);
            draw_hline(sx - 4, sx + 4, sy, d);
            draw_hline(sx - 4, sx + 4, sy + 1, d);
            draw_hline(sx - 3, sx + 3, sy + 2, d);
            draw_hline(sx - 3, sx + 3, sy + 3, d);
            draw_hline(sx - 1, sx + 1, sy + 4, d);
        }
        break;
    case 6:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 2, sx + 2, sy - 5, d);
            draw_hline(sx - 4, sx + 4, sy - 4, d);
            draw_hline(sx - 4, sx + 4, sy - 3, d);
            draw_hline(sx - 5, sx + 5, sy - 2, d);
            draw_hline(sx - 5, sx + 5, sy - 1, d);
            draw_hline(sx - 5, sx + 5, sy, d);
            draw_hline(sx - 5, sx + 5, sy + 1, d);
            draw_hline(sx - 5, sx + 5, sy + 2, d);
            draw_hline(sx - 4, sx + 4, sy + 3, d);
            draw_hline(sx - 4, sx + 4, sy + 4, d);
            draw_hline(sx - 2, sx + 2, sy + 5, d);
        }
        break;
    case 7:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 2, sx + 2, sy - 6, d);
            draw_hline(sx - 4, sx + 4, sy - 5, d);
            draw_hline(sx - 5, sx + 5, sy - 4, d);
            draw_hline(sx - 5, sx + 5, sy - 3, d);
            draw_hline(sx - 6, sx + 6, sy - 2, d);
            draw_hline(sx - 6, sx + 6, sy - 1, d);
            draw_hline(sx - 6, sx + 6, sy, d);
            draw_hline(sx - 6, sx + 6, sy + 1, d);
            draw_hline(sx - 6, sx + 6, sy + 2, d);
            draw_hline(sx - 5, sx + 5, sy + 3, d);
            draw_hline(sx - 5, sx + 5, sy + 4, d);
            draw_hline(sx - 4, sx + 4, sy + 5, d);
            draw_hline(sx - 2, sx + 2, sy + 6, d);
        }
        break;
    case 8:
        {
            int sx, sy;
            sx = (short)px; sy = (short)py;
            draw_hline(sx - 2, sx + 2, sy - 7, d);
            draw_hline(sx - 4, sx + 4, sy - 6, d);
            draw_hline(sx - 5, sx + 5, sy - 5, d);
            draw_hline(sx - 6, sx + 6, sy - 4, d);
            draw_hline(sx - 6, sx + 6, sy - 3, d);
            draw_hline(sx - 7, sx + 7, sy - 2, d);
            draw_hline(sx - 7, sx + 7, sy - 1, d);
            draw_hline(sx - 7, sx + 7, sy, d);
            draw_hline(sx - 7, sx + 7, sy + 1, d);
            draw_hline(sx - 7, sx + 7, sy + 2, d);
            draw_hline(sx - 6, sx + 6, sy + 3, d);
            draw_hline(sx - 6, sx + 6, sy + 4, d);
            draw_hline(sx - 5, sx + 5, sy + 5, d);
            draw_hline(sx - 4, sx + 4, sy + 6, d);
            draw_hline(sx - 2, sx + 2, sy + 7, d);
        }
        break;
    }
}
