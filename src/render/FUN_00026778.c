/* @ 0x26778 (544 bytes): dashed Bresenham line drawer, plotting every 2nd pixel
   from (x1,y1) to (x2,y2) via the pixel-plot callee plot_point(x,y,color).
   Color alternates between c1 and c2 on bit 2 of the ushort pattern counter a7
   (a7 is decremented per step on shallow lines, incremented on steep ones, so
   4-pixel dashes of each color). Classic two-branch integer Bresenham: shallow
   (|dx| >= |dy|) walks x in +/-2 steps stepping y by +/-2 on error overflow;
   steep mirrors with x/y swapped. Coordinates are treated as unsigned shorts.

   PARKED near-miss 529B vs 544B target (NOT matched), best 358 differing
   instruction lines over 9 compiles; structure is block-for-block 1:1 (frame
   sub esp,0x20 matches, both loops rotate correctly, dec/inc word [a7-slot],
   ternary color select, all four sub-cases + shared epilogue). KEY WINS:
   - `int two = 2;` named local is load-bearing: it homes the constant 2 in EBP
     (target `mov ebp,2`, `add esi,ebp` loop increments, and the step negation
     reading the CSE via `mov ecx,ebp; neg ecx`), demotes stepa/stepb to stack
     slots, and grows the frame to the target's 8 slots. Without it the steps
     registerize into EBP and the +/-2 constant-folds.
   - ushort x1,y1 params + int x2,y2: deranks x1/y1 from scratch promotion so
     x2/y2 get cached in registers like the target (all-int params promoted
     x1/y1 instead).
   - step written arm-duplicated (`if (c) { s = two; s = -s; } else { s = two; }`)
     reproduces the target's store-2-then-store-neg shape in the negative arm.
   REMAINING WALL: the scratch-promotion register pool. Ours caches x2->EDX,
   y2->EAX with the (ushort) zext temps in EBX; target caches x2->ECX, y2->EDX
   keeping EAX for the zext temps. That one seed renames nearly every register
   downstream (dx/dy homes, cmp cx/dx orientations, slot-assignment scramble for
   the 8 locals), which is why the LCS byte score (221/544) looks worse than the
   structural closeness. Tried: named int copies (land EDX,EAX regardless of decl
   order/position), py-only copy (EAX), ushort copies (worse). If the pool seed
   can be flipped (permuter statement reorder?), expect most of the body to snap
   in; slot order then needs decl-order juggling (reverse-decl mapping held in
   attempt 2 but is perturbed by copy decls). */

extern void plot_point(int x, int y, int c);

void FUN_00026778(unsigned short x1, unsigned short y1, int x2, int y2,
                  unsigned char c1, unsigned char c2, unsigned short a7)
{
    int stepa;
    int stepb;
    int inc1a;
    int inc2a;
    int inc1b;
    int inc2b;
    int end1;
    int end2;
    int i;
    int j;
    int two;
    int err;
    int dx;
    int dy;

    two = 2;
    dx = (unsigned short)x2 - x1;
    if (dx < 0) {
        dx = -dx;
    }
    dy = (unsigned short)y2 - y1;
    if (dy < 0) {
        dy = -dy;
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
