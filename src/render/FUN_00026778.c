/* @ 0x26778 (544 bytes): render_dashed_line -- dashed Bresenham line drawer,
   plotting every 2nd pixel from (x1,y1) to (x2,y2) via the pixel-plot callee
   plot_point(x,y,color). Color alternates between c1 and c2 on bit 2 of the
   ushort pattern counter a7 (a7 decremented per step on shallow lines,
   incremented on steep ones, so 4-pixel dashes of each color). Classic
   two-branch integer Bresenham: shallow (|dx| >= |dy|) walks x in +/-2 steps
   stepping y by +/-2 on error overflow; steep mirrors with x/y swapped.
   Coordinates are treated as unsigned shorts. Proposed name: render_dashed_line.

   NEAR-MATCH, EDIT-DIST 139 (was 157), 529B vs 544B target (NOT matched).
   Structure is block-for-block 1:1 (frame sub esp,0x20 matches, both loops
   rotate correctly, dec/inc word [a7-slot], ternary color select, all four
   sub-cases + shared epilogue). KEY WINS (this session, on top of the earlier
   baseline):
   - Computing dy BEFORE dx (source-faithful reorder of two independent abs
     legs) flips y2 into EDX to match the target (was y2->EAX); -12 dist.
   - Reversing the local declaration order pulls the 8 spill slots toward the
     target's layout (e.g. stepa now homes at [esp+0x1c] like the target); -6.
   EARLIER WINS (retained):
   - `int two = 2;` named local homes the constant 2 in EBP (target `mov ebp,2`,
     `add esi,ebp` loop increments, `mov ecx,ebp; neg ecx` step negation),
     demotes stepa/stepb to slots, grows the frame to the target's 8 slots.
   - ushort x1,y1 params + int x2,y2 derank x1/y1 from scratch promotion so
     x2/y2 cache in registers.
   - step written arm-duplicated (`if (c){s=two; s=-s;} else {s=two;}`)
     reproduces the store-2-then-store-neg shape.
   REMAINING WALL: the scratch-promotion register pool (ECX vs EBX). Target
   caches x2->ECX with the (ushort) zext temps in EAX; ours puts x2->EAX and
   pushes the zext temps into the callee-saved EBX -- ours never draws ECX into
   this pool. That one seed renames nearly every downstream register byte and
   also swaps dx/dy (dy-first gives dy=ESI/dx=EDI, target dx=ESI/dy=EDI), which
   is why the byte score lags the structural 1:1 closeness. The two orderings
   are coupled: dx-first fixes dx/dy but breaks the y2 param seed; dy-first
   fixes y2 but swaps dx/dy -- neither reaches ECX. Tried this session: int
   x1/y1 (worse, 194), dx-first + reversed decls (151), `dy<=dx` compare (inert),
   several decl permutations (end1/end2 swap 153, inc-pair swaps inert). Not
   source-reachable through these levers -- allocator-internal ECX-pool floor. */

extern void plot_point(int x, int y, int c);

void FUN_00026778(unsigned short x1, unsigned short y1, int x2, int y2,
                  unsigned char c1, unsigned char c2, unsigned short a7)
{
    int dy;
    int dx;
    int err;
    int two;
    int j;
    int i;
    int end2;
    int end1;
    int inc2b;
    int inc1b;
    int inc2a;
    int inc1a;
    int stepb;
    int stepa;

    two = 2;
    dy = (unsigned short)y2 - y1;
    if (dy < 0) {
        dy = -dy;
    }
    dx = (unsigned short)x2 - x1;
    if (dx < 0) {
        dx = -dx;
    }
    if (dx >= dy) {
        inc2a = dy * 2;
        err = dy * 2 - dx;
        inc1a = (dy - dx) * 2;
        if ((unsigned short)x2 < x1) {
            i = (unsigned short)x2;
            end1 = x1;
            j = (unsigned short)y2;
            if ((unsigned short)y2 > y1) {
                stepa = two;
                stepa = -stepa;
            } else {
                stepa = two;
            }
        } else {
            i = x1;
            end1 = (unsigned short)x2;
            j = y1;
            if ((unsigned short)y2 < y1) {
                stepa = two;
                stepa = -stepa;
            } else {
                stepa = two;
            }
        }
        plot_point(i, j, (a7 & 4) ? c2 : c1);
        while (i < end1) {
            i += two;
            if (err < 0) {
                err += inc2a;
            } else {
                j += stepa;
                err += inc1a;
            }
            plot_point(i, j, (a7 & 4) ? c2 : c1);
            a7--;
        }
    } else {
        inc2b = dx * 2;
        err = dx * 2 - dy;
        inc1b = (dx - dy) * 2;
        if ((unsigned short)y2 < y1) {
            i = (unsigned short)y2;
            end2 = y1;
            j = (unsigned short)x2;
            if ((unsigned short)x2 > x1) {
                stepb = two;
                stepb = -stepb;
            } else {
                stepb = two;
            }
        } else {
            i = y1;
            end2 = (unsigned short)y2;
            j = x1;
            if ((unsigned short)x2 < x1) {
                stepb = two;
                stepb = -stepb;
            } else {
                stepb = two;
            }
        }
        plot_point(j, i, (a7 & 4) ? c2 : c1);
        while (i < end2) {
            i += two;
            if (err < 0) {
                err += inc2b;
            } else {
                j += stepb;
                err += inc1b;
            }
            plot_point(j, i, (a7 & 4) ? c2 : c1);
            a7++;
        }
    }
}
