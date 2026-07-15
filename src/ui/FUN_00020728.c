/* @ 0x20728: menu list draw + select, twin of 0x205f8. Walks the 501-byte
   records at g_7bf4 (byte +0 = type/terminator 0xff; word +0x11 = state),
   drawing a text row via FUN_36698 for each record whose state == 0x960, at
   x=0x1f8 starting y=0x70 stepping 12, colour 0xdd, using string ptr
   g_4b10[g_list_recs[i*0x1eb]*3 + g_language] (type byte still from the 0x1eb-strided
   pool). Then runs the list-select loop FUN_20018 (id 0x14 here vs 0x12); if a   row was picked (nonzero 1-based result), redraws it highlighted (colour
   0x54) at y = sel*12 + 0x70.

   NEAR-MISS (NOT matched). Register-ROLE wall, same as its twin 0x205f8: the
   target uses FOUR callee-saved regs (pushes ebp) and swaps the edx/eax roles in
   the record-index loop, where our Watcom uses three. Entry `push ebp` is the first
   diff -- not source-reachable. */

extern unsigned char g_7bf4[];
extern unsigned char g_list_recs[];
extern char *g_4b10[];
extern char *g_4b10_2[][3];
extern unsigned char g_language;
extern unsigned char *g_text_pal;
extern unsigned short g_mouse_x;
extern unsigned short g_mouse_y;

extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern int FUN_00020018(int, int, int, unsigned short *, int, int, int, int, int, int);

void FUN_00020728(void)
{
    unsigned short r;
    unsigned short y;
    unsigned short i;
    unsigned short sel;

    y = 0x70;
    for (i = 0;; ++i) {
        unsigned int wi = i;
        if (g_7bf4[wi * 0x1f5] == 0xff)
            break;
        if (*(unsigned short *)(g_7bf4 + wi * 0x1f5 + 0x11) == 0x960) {
            FUN_00036698(*(char **)((char *)g_4b10 + g_language * 4 + g_list_recs[wi * 0x1eb] * 12),
                         0x1f8, y, 0xe, 0xdd, g_text_pal, -2, 6, 0, 0);
            y += 0xc;
        }
    }
    if ((r = FUN_00020018(0x14, g_mouse_x, g_mouse_y, &sel, 0, 0xfa, 0x35, 0x139, 0xab, 0x38)) != 0) {
        FUN_00036698(g_4b10_2[g_list_recs[(r - 1) * 0x1eb]][g_language],
                     0x1f8, (unsigned short)(sel * 0xc + 0x70), 0xe, 0x54,
                     g_text_pal, -2, 6, 0, 0);
    }
}
