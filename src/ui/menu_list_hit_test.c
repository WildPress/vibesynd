/* PARKED near-miss @ 0x20018 -- 307/314 bytes, sizes 307 vs 314, recipe
   -4s -oneatx -zp8 -s -zq. All structure, immediates, guards, both pool loops,
   switch tree, and epilogue shapes correct; residue is ONE allocation bit plus
   an entry-scheduler pair (12 iterations):
   (1) row/out 2-cycle: target homes row->DL, out-ptr->ECX (divisor ECX reused);
       ours always row->CL, out->EDX, which cascades: case-0x14 CSE temp
       CX->DX, case-0x12 named st DI->DX (push/pop edi vanish, every [esp+N]
       param offset shifts -4), flag copy DH->CH, movsx scratch ECX<->EDX.
       Tried: named q=out (decl before/after row), int t quotient temp (t gets
       out->ECX right but materialises mov edx,eax), anonymous CSE store
       (division duplicates), uchar row + (signed char) casts, out as int
       param -- allocator insensitive or anti-correlated in all spellings.
   (2) entry pair: target has n load (mov esi,[esp+0x10]) FIRST and guard-1
       x0*2 in the sequential lea form (xor eax; mov ax,x0; lea edx,[eax*2+0]).
       A named ushort w + a materialised int local (dy) reproduce the lea form
       exactly, but then w's load schedules BEFORE n's; without the int local
       n comes first but guard-1 degrades to the interleaved add-form
       (xor edx; xor eax; mov dx,x0; mov ax,x; add edx,edx). Anti-correlated
       in every spelling tried (same class as the 0x128b8 wall).

   @ 0x20018: menu list-select hit test. Mouse (x,y) vs a doubled-coordinate
   box (x0*2, y0*2)..(x1*2, y1*2); row = (y - ytop*2)/12 stored to *out
   (zero-extended byte). Then walks the record pool for the list (count 0x14 ->
   pool g_7bf4, stride 0x1f5, state word +0x11; count 0x12 -> pool g_list_recs,
   stride 0x1eb, state word +8), counting records; a record is "visible" when
   flag==0 ? state == 0x960 : 0 <= (short)state < 100. Decrements row per
   visible record; returns the 1-based record index where row ran out, or 0. */
extern unsigned char g_list_recs[];
extern unsigned char g_7bf4[];

int menu_list_hit_test(int a1, unsigned short x, unsigned short y, unsigned short *out,
                 int flag, unsigned short x0, unsigned short y0,
                 unsigned short x1, unsigned short y1, unsigned short ytop)
{
    unsigned short n;
    int k = 0;
    int dy;
    unsigned char row;
    unsigned char *p;
    unsigned char f;
    unsigned short w;

    n = a1;
    w = x0;
    if (x <= w * 2) goto done;
    if (x >= x1 * 2) goto done;
    if (y <= y0 * 2) goto done;
    if (y >= y1 * 2) goto done;
    dy = y - ytop * 2;
    row = (unsigned char)(dy / 12);
    if (out != 0)
        *out = row;
    if ((signed char)row >= n) goto done;
    switch (n) {
    case 0x14:
        p = g_7bf4;
        f = flag;
        for (;;) {
            if ((signed char)row <= -1) goto chk;
            ++k;
            if ((unsigned short)k > n) goto chk;
            if (f == 0) {
                if (*(unsigned short *)(p + 0x11) != 0x960) goto next14;
            } else {
                if (*(short *)(p + 0x11) < 0) goto next14;
                if (*(short *)(p + 0x11) >= 0x64) goto next14;
            }
            --row;
        next14:
            p += 0x1f5;
        }
    case 0x12:
        p = g_list_recs;
        for (;;) {
            if ((signed char)row <= -1) goto chk;
            ++k;
            if ((unsigned short)k > n) goto chk;
            if ((unsigned char)flag == 0) {
                if (*(unsigned short *)(p + 8) != 0x960) goto next12;
            } else {
                short st = *(short *)(p + 8);
                if (st < 0) goto next12;
                if (st >= 0x64) goto next12;
            }
            --row;
        next12:
            p += 0x1eb;
        }
    }
chk:
    if ((unsigned short)k > n) k = 0;
done:
    return k;
}
