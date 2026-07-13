/* @ 0x36698: the UI text drawer. Walks the NUL-terminated string s one byte at
   a time (byte index i). '\n' resets the pen x to the x param and advances y by
   a4 (the line height). For every other char the glyph record is the 6-byte
   entry tbl + (c - 0x20 + a5) * 6 (a5 = font/colour bank offset; +4 = advance
   width, +5 = height). If a9 (underline colour) is nonzero, first draws a line
   via FUN_18ae8 from (x, y+a4-2) to (x + width + a7, y+a4-2) in colour a9.
   Space (c == 0x20) just advances the pen by the bank-0 glyph width + a7
   (a7 = per-char kerning, -2 at most call sites). Otherwise blits the glyph
   with FUN_4a6c8 at (x, y + 12 - height), temporarily swapping the back-buffer
   pointer g_5368 to g_5370 around the blit when a10 is set. a8 is unused. */
extern unsigned char *g_5368;
extern unsigned char *g_5370;

extern void FUN_00018ae8(int x1, int y1, int x2, int y2, int c);
extern void FUN_0004a6c8(int x, int y, unsigned char *spr);

void FUN_00036698(char *s, unsigned short x, unsigned short y, unsigned char a4,
                  unsigned short a5, unsigned char *tbl, signed char a7,
                  signed char a8, unsigned char a9, unsigned char a10)
{
    unsigned char i;
    unsigned char j;
    unsigned short xpos;
    unsigned short ypos;
    unsigned char *saved;
    unsigned char *font;
    unsigned char *glyph;

    font = tbl;
    xpos = x;
    i = 0;
top:
    if (s[i] == 0)
        goto done;
    {
        if (s[i] == 0xa) {
            y += a4;
            xpos = x;
            goto next;
        }
        j = s[i];
        if (a9 != 0) {
            FUN_00018ae8((short)xpos, (short)(a4 + y - 2),
                         (short)(a7 + (font[((unsigned)j + a5 - 0x20) * 6 + 4] + xpos)),
                         (short)(a4 + y - 2), a9);
        }
        if (j == 0)
            goto done;
        j -= 0x20;
        if (j != 0) {
            if (a10) {
                saved = g_5368;
                g_5368 = g_5370;
            }
            glyph = font + (j + a5) * 6;
            ypos = y + 0xc - glyph[5];
            FUN_0004a6c8(xpos, ypos, glyph);
            xpos += a7 + glyph[4];
            if (a10)
                g_5368 = saved;
        } else {
            xpos += font[a5 * 6 + 4] + a7;
        }
    }
next:
    ++i;
    goto top;
done:;
}
