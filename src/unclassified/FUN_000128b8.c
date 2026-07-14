/* PARKED near-miss @ 0x128b8 -- 317/353 bytes, first diff 0x1a, size match
   353/353, all fixup sites aligned. Register-role wall (see below).

   Semantics: find a live ped (pool-A type 1) near (x,y,z). Scans the 3x3
   g_10e grid cells whose top-left is the tile of (x - rx/2 - 0x100,
   y - ry/2 - 0x100), walking each cell's id chain (bounded at 0x400 nodes).
   A node matches when type[0x18]==1, flag bit (node[0xb] & type) clear,
   (link word +0x1c & mask) != 0, and its coords fall inside the box
   x +/- (rx/2 + 0x20), y +/- (ry/2 + 0x20), z - 0x100 <= nz <= z + rz.
   First param is unused (target reads args from [esp+0x28] up).
   Returns the node pointer, or 0. Recipe: -4s -oneatx -zp8 -s -zq.

   WALL: two coupled one-bit allocation choices, anti-correlated in every
   source spelling tried (10 iterations):
   (1) homes of the loop-invariant halves: target hry->ESI, hrx->ECX
       (hrx's home copy deferred to after the p= statement, i.e. CSE of an
       inline rx/2). Named-cy / deferred-hrx / all-inline forms all give
       hry->ECX, hrx->ESI; only the both-eager form (hry=; hrx=; p=;)
       gives the target homes -- but that form also flips (2).
   (2) guard temp pairing edi/ebp + cmp orientation in checks 1-3: the
       goto-guard body with swapped homes produces the target's exact
       cmp/jcc shapes (param-copy->EBP jl for the x checks, param-copy->EDI
       jg for the y checks); with correct homes the temps flip to
       (edi,ecx)/(ebp,esi) and the x-check cmp reverses to jg.
   Also iy/ix spill slots come out 4/0xc vs target 0xc/4 (i@8 correct);
   slot choice tracked neither declaration nor init order.
   Levers that DID close diffs and are load-bearing here: dummy first param;
   byte-cast (unsigned char)(node[0xb] & t) to force the byte test
   test [eax+0xb],dl instead of a zero-extended dword test; goto-style
   negative guards (direct jl/jg senses) instead of an && chain; named t
   for the type byte; i++/id-reload/bound-check order in the chain walk. */
extern unsigned short g_10e[];
extern unsigned char g_810e[];

unsigned char *FUN_000128b8(int unused, unsigned short mask, short x, short y,
                            short z, int rx, int ry, int rz)
{
    int cy;
    int hry;
    int hrx;
    unsigned short *p;
    unsigned short iy;
    unsigned short ix;
    unsigned short i;
    unsigned short id;
    unsigned char *node;
    unsigned char t;

    hry = ry / 2;
    cy = y - hry;
    p = g_10e + ((((cy - 0x100) & 0x7f00) >> 1) | (((x - rx / 2 - 0x100) >> 8) & 0x7f));
    hrx = rx / 2;
    for (iy = 0; iy < 3; iy++) {
        for (ix = 0; ix < 3; ix++) {
            i = 0;
            id = *p;
            if (id != 0) {
                do {
                    node = g_810e + id;
                    t = node[0x18];
                    if (t != 1)
                        goto skip;
                    if ((unsigned char)(node[0xb] & t))
                        goto skip;
                    if (!(*(unsigned short *)(node + 0x1c) & mask))
                        goto skip;
                    if (*(short *)(node + 4) + 0x20 < x - hrx)
                        goto skip;
                    if (*(short *)(node + 4) - 0x20 > x + hrx)
                        goto skip;
                    if (y - hry > *(short *)(node + 6) + 0x20)
                        goto skip;
                    if (*(short *)(node + 6) - 0x20 > y + hry)
                        goto skip;
                    if (*(short *)(node + 8) + 0x100 < z)
                        goto skip;
                    if (*(short *)(node + 8) <= z + rz)
                        return node;
                skip:
                    i++;
                    id = *(unsigned short *)node;
                    if (i >= 0x400)
                        break;
                } while (id != 0);
            }
            p++;
        }
        p += 0x7d;
    }
    return 0;
}
