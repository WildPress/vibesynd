/* draw_circle @ 0x19318 - draw a circle outline centred (cx,cy), radius r,
 * colour c, by midpoint/Bresenham 8-way symmetry: per step plots the 8 octant
 * points via the pixel-plot 0x40236, decision d = 3 - 2r, d += 4x+6 (d<0) or
 * d += 4(x-y)+10 with y -= step. step is 1, or 2 when g_105 has bit 1 clear
 * and any of bits 0/2 set (halved vertical-res video modes). After the loop a
 * final 8-point pass fires when x == y (the diagonal octant seam).
 *
 * PARKED at 589B vs 585B (-oneatx). Bytes 0x00-0x115 (entry, guard, all 8
 * loop plot calls) are instruction-identical modulo the frame size (ours
 * 0x20 vs 0x1c => every local disp8 is +4). Register-role tie-break wall,
 * one root: after call 8 the target reloads d into ESI (mov esi,[d]) leaving
 * EAX free for step inside the else, so new-y (EDX) stores BEFORE d and both
 * branch tails cross-jump onto one shared `mov [d],ebx` (if-branch = bare
 * jmp). Ours reloads d into EAX, serialises step through EAX after it, flips
 * the else store order ([d] then [y]) and loses the merge (+4B duplicated
 * store). Downstream of the same pick: loop-bottom x reload EAX->ECX, the
 * x==y compare y reload ECX->EDI, the whole block-2 callee-saved rotation
 * (cy EDI->EBP, cx ESI->EDI, y EBP->ESI) and a 4th spill-temp slot in block 2
 * (target reuses the dead cy-y slot for cx+x) = the frame delta. Entry-only
 * residue: ours hoists d's 3 into EDI first and zeroes x via EDX; target
 * zeroes EDI for x then reuses EDI for 3 (size-neutral).
 * Tried: statement permutations of x/y/d init (byte-inert), else via named
 * old-y short temp (temp steals a slot, wrecks layout), int dword-alias
 * `*(int *)&d` reload (frame shrinks to 0x1c but d lands in EBX, d's entry
 * store goes word, slots re-rank), for-header loop (+2B), -or (loop shape
 * diverges, 572B), -ot (much worse). 10 compiles.
 * Recipe: -4s -oneatx -zp8 -s -zq (parked) */
extern unsigned char g_105;
extern void FUN_00040236(int x, int y, int c);   /* plot pixel */

void draw_circle(short cx, short cy, short r, unsigned char color)
{
    short x;
    short y;
    short d;
    int step;

    step = 1;
    if (!(g_105 & 2) && (g_105 & 5))
        step = 2;
    x = 0;
    y = r;
    d = 3 - (r + r);
    while (x < y) {
        FUN_00040236(cx - x, cy - y, color);
        FUN_00040236(cx + x, cy - y, color);
        FUN_00040236(cx - x, cy + y, color);
        FUN_00040236(cx + x, cy + y, color);
        FUN_00040236(cx - y, cy - x, color);
        FUN_00040236(cx + y, cy - x, color);
        FUN_00040236(cx - y, cy + x, color);
        FUN_00040236(cx + y, cy + x, color);
        if (d < 0) {
            d = x * 4 + d + 6;
        } else {
            d = (x - y) * 4 + d + 10;
            y -= step;
        }
        x += step;
    }
    if (x == y) {
        FUN_00040236(cx - x, cy - y, color);
        FUN_00040236(cx + x, cy - y, color);
        FUN_00040236(cx - x, cy + y, color);
        FUN_00040236(cx + x, cy + y, color);
        FUN_00040236(cx - y, cy - x, color);
        FUN_00040236(cx + y, cy - x, color);
        FUN_00040236(cx - y, cy + x, color);
        FUN_00040236(cx + y, cy + x, color);
    }
}
