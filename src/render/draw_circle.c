/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): pure CODEGEN TIE. Full aligned
 * walk confirms identical arithmetic (d=3-2r, +4x+6, +4(x-y)+10, x/y +=step), all
 * branch conditions (x<y loop, d<0 test, x==y seam) and all 16 plot_point calls
 * with matching args. Divergences are register-allocation only (the target holds d
 * in a callee-saved reg across the d<0 test; ours cmp-in-memory then reloads) plus
 * downstream register naming. Target-side garbage after the block-2 calls in the
 * regdiff --show is just the disassembler desyncing on masked call displacements.
 *
 * draw_circle @ 0x19318 - draw a circle outline centred (cx,cy), radius r,
 * colour c, by midpoint/Bresenham 8-way symmetry: per step plots the 8 octant
 * points via the pixel-plot 0x40236, decision d = 3 - 2r, d += 4x+6 (d<0) or
 * d += 4(x-y)+10 with y -= step. step is 1, or 2 when g_105 has bit 1 clear
 * and any of bits 0/2 set (halved vertical-res video modes). After the loop a
 * final 8-point pass fires when x == y (the diagonal octant seam).
 *
 * NEAR-MATCH, 592B vs 585B, EDIT-DIST 88 (was 165 with `int step`).
 * KEY FIX: `step` must be `short`, not `int`. That single change collapses
 * the frame from 0x20 to 0x1c (target size), aligns every local disp8 and the
 * whole entry/guard/8-plot body (bytes 0x00-0x2c now identical), and drops the
 * distance 165 -> 88. Block 2 (the x==y diagonal pass) is now byte-identical
 * to target except one ECX-vs-EDX naming tie on the y reload (3B).
 * Remaining gap = register-allocation ties around the d<0 decision, one root:
 * target loads d into a callee-saved reg at the test (`mov esi,[d]; test
 * si,si`) and reuses it in BOTH branches, no reloads. Ours tests d in memory
 * (`cmp word[d],0`) then RELOADS d in each branch (+1 insn / +7B: the whole
 * length delta). From that pick flow the downstream naming ties (else y EDI->
 * ECX, loop-bottom x EAX->EDI, block-2 y ECX->EDX) and the entry residue
 * (target zeroes EDI for x then reuses EDI for the constant 3; ours zeroes
 * ESI). Store order (new-y before d) and the shared `mov [d],ebx` merge both
 * MATCH now. Tried (all >= 88): `int od=d` hoist (116, int sign-extend re-ranks
 * slots), `short od=d` hoist (byte-inert, compiler folds it), init reorder
 * y/d/x (99), else with saved old-y (byte-trick, no help). The mem-cmp-vs-
 * register-load-of-d decision is a Watcom codegen tie not reachable from C.
 * Recipe: -4s -oneatx -zp8 -s -zq */
extern unsigned char g_105;
extern void plot_point(int x, int y, int c);   /* plot pixel */

void draw_circle(short cx, short cy, short r, unsigned char color)
{
    short x;
    short y;
    short d;
    short step;

    step = 1;
    if (!(g_105 & 2) && (g_105 & 5))
        step = 2;
    x = 0;
    y = r;
    d = 3 - (r + r);
    while (x < y) {
        plot_point(cx - x, cy - y, color);
        plot_point(cx + x, cy - y, color);
        plot_point(cx - x, cy + y, color);
        plot_point(cx + x, cy + y, color);
        plot_point(cx - y, cy - x, color);
        plot_point(cx + y, cy - x, color);
        plot_point(cx - y, cy + x, color);
        plot_point(cx + y, cy + x, color);
        if (d < 0) {
            d = x * 4 + d + 6;
        } else {
            d = (x - y) * 4 + d + 10;
            y -= step;
        }
        x += step;
    }
    if (x == y) {
        plot_point(cx - x, cy - y, color);
        plot_point(cx + x, cy - y, color);
        plot_point(cx - x, cy + y, color);
        plot_point(cx + x, cy + y, color);
        plot_point(cx - y, cy - x, color);
        plot_point(cx + y, cy - x, color);
        plot_point(cx - y, cy + x, color);
        plot_point(cx + y, cy + x, color);
    }
}
