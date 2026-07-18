/* PARKED near-miss @ 0x18ae8 -- 516/524 bytes (aligned-common ~485/524 after
   fixup masking), first diff 0x6f, whole CFG/branch layout byte-correct.

   WALL 1 (primary, 8-byte size deficit): the target materialises 2*minor as a
   7-byte scaled-index LEA with zero disp32 (8d047500000000 lea eax,[esi*2+0]
   x-major; 8d047d.. [edi*2+0] y-major). Our 9.5b ALWAYS strength-reduces *2 to
   add eax,eax (x-major, via copy-prop from the labs return) / mov eax,edi;
   add eax,eax (y-major). Not source-reachable: `dy * 2`, `dy << 1`,
   `(short*)0 + dy`, and an extern-symbol base `g_zero + dy` (single-use and
   CSE'd) all fail -- the symbol forms do NOT fold sym+idx*2 into one lea,
   they emit mov edx,sym; lea ebx,[eax*2+0]; add and restructure the entry.
   Same family as the push imm8-vs-imm32 peephole wall (0x16678).
   WALL 2: the incE/incNE/incE2/incNE2 spill quartet sits in a 3-cycle
   (ours incNE->0x14, incNE2->0x10, incE2->0x1c vs target 0x1c/0x14/0x10;
   incE->0x18 correct). Decl-order changes move these slots NON-locally
   (swapping two decls swaps a different pair); 3 permutations tried.
   All other slots (y1i/x2i/y2i/x1i, end/end2, ystep/xstep) were steered to
   target via decl order -- here decl order is NOT byte-inert, unlike 0x338d8.

   Semantics: Bresenham line draw from (x1,y1) to (x2,y2) in colour c, plotting
   each point via FUN_00040236(x, y, c). Step size comes from the mode byte
   g_105: bit 1 -> 1, else bits 0/2 -> 2 (uninitialised otherwise, as original).
   Splits into an x-major and a y-major half, each walking from the smaller
   endpoint of the major axis with the classic 2*minor error accumulator
   (d = 2*minor - major, incE = 2*minor, incNE = 2*(minor - major)), stepping
   the minor coord by +/-step. Calls labs (labs) for the deltas.
   Recipe: -4s -oneatx -zp8 -s -zq. */
extern unsigned char g_105;
long labs(long x);
void FUN_00040236(int x, int y, int c);

void FUN_00018ae8(short x1, short y1, short x2, short y2, unsigned char c)
{
    int step;
    int dx;
    int dy;
    int y1i;
    int x2i;
    int x1i;
    int y2i;
    int d;
    int p;
    int q;
    int incNE;
    int incNE2;
    int incE;
    int incE2;
    int end;
    int end2;
    int xstep;
    int ystep;

    if (g_105 & 2)
        step = 1;
    else if (g_105 & 5)
        step = 2;
    x2i = x2;
    x1i = x1;
    dx = labs(x2i - x1i);
    y2i = y2;
    y1i = y1;
    dy = labs(y2i - y1i);
    if (dx >= dy) {
        incE = dy * 2;
        d = dy * 2 - dx;
        incNE = (dy - dx) * 2;
        if (x1 > x2) {
            p = x2i;
            end = x1i;
            q = y2i;
            if (y2 > y1) {
                ystep = step;
                ystep = -ystep;
                goto go1;
            }
        } else {
            p = x1i;
            end = x2i;
            q = y1i;
            if (y1 > y2) {
                ystep = step;
                ystep = -ystep;
                goto go1;
            }
        }
        ystep = step;
    go1:
        FUN_00040236(p, q, c);
        while (p < end) {
            p += step;
            if (d < 0) {
                d += incE;
            } else {
                q += ystep;
                d += incNE;
            }
            FUN_00040236(p, q, c);
        }
    } else {
        incE2 = dx * 2;
        d = dx * 2 - dy;
        incNE2 = (dx - dy) * 2;
        if (y1 > y2) {
            q = x2i;
            end2 = y1i;
            p = y2i;
            if (x2 > x1) {
                xstep = step;
                xstep = -xstep;
                goto go2;
            }
        } else {
            q = x1i;
            end2 = y2i;
            p = y1i;
            if (x1 > x2) {
                xstep = step;
                xstep = -xstep;
                goto go2;
            }
        }
        xstep = step;
    go2:
        FUN_00040236(q, p, c);
        while (p < end2) {
            p += step;
            if (d < 0) {
                d += incE2;
            } else {
                q += xstep;
                d += incNE2;
            }
            FUN_00040236(q, p, c);
        }
    }
}
