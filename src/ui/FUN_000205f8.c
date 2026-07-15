/* @ 0x205f8: menu list draw + select. Walks the 491-byte records at g_5780
   (byte +0 = type, terminator 0xff; word +8 = state), drawing a text row via
   FUN_36698 for each record whose state == 0x960, at x=0x1f8 starting y=0x70
   stepping 12, colour 0xdd, using string ptr g_4a38[type*3 + g_language]. Then runs
   the list-select loop FUN_20018 over the drawn rows; if a row was picked
   (nonzero 1-based result), redraws that row highlighted (colour 0x54) at
   y = sel*12 + 0x70.

   NEAR-MISS (NOT matched). Register-ROLE wall: the target uses FOUR callee-saved
   registers (pushes ebp as well as ebx/esi/edi) and routes the two loaded arg dwords
   through ebp/ebx, where our Watcom uses three (edx/ecx). First diff is the entry
   `push ebp` -- no C form makes the allocator reserve a 4th callee-saved reg here. */
extern unsigned char g_5780[];
extern char *g_4a38[];
extern unsigned char g_language;
extern unsigned char *g_11be4;
extern unsigned short g_5390;
extern unsigned short g_5392;

extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern int FUN_00020018(int, int, int, unsigned short *, int, int, int, int, int, int);

void FUN_000205f8(void)
{
    unsigned short r;
    unsigned short y;
    unsigned short i;
    unsigned short sel;

    y = 0x70;
    for (i = 0;; ++i) {
        if (g_5780[i * 0x1eb] == 0xff)
            break;
        if (*(unsigned short *)(g_5780 + i * 0x1eb + 8) == 0x960) {
            FUN_00036698(*(char **)((char *)g_4a38 + g_language * 4 + g_5780[i * 0x1eb] * 12),
                         0x1f8, y, 0xe, 0xdd, g_11be4, -2, 6, 0, 0);
            y += 0xc;
        }
    }
    if ((r = FUN_00020018(0x12, g_5390, g_5392, &sel, 0, 0xfa, 0x35, 0x139, 0xab, 0x38)) != 0) {
        FUN_00036698(*(char **)((char *)g_4a38 + g_language * 4 + g_5780[(r - 1) * 0x1eb] * 12),
                     0x1f8, (unsigned short)(sel * 0xc + 0x70), 0xe, 0x54,
                     g_11be4, -2, 6, 0, 0);
    }
}
