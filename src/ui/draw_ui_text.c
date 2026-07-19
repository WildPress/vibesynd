/* @ 0x36698: near-miss, EDIT-DIST 37 / 362 bytes (down from 83), -4s -oneatx
   -zp8 -s -zq. Two structural levers cracked the old "spill-slot" and
   "underline register-role" walls the previous park thought were ties:

   (1) Spill-slot order (target i@[esp+0], j@[esp+4]) is NOT fixed under the
   goto/test-at-top loop by itself — but adding the extra byte local `y2` (used
   for glyph[5]) plus keeping `ypos` as a live local shifts Watcom's temp
   ranking so i lands in slot 0 exactly like the target. `y2` must be
   `unsigned char` (a `short` y2 forces a spurious `xor ax,ax` widen); and the
   glyph-block `ypos` local must stay (inlining it drops the temp and the slot
   swap returns). The `for(;;)` form still converts to jump-to-test, so the
   goto/test-at-top shape is kept.
   (2) Underline register-role (target y2->EDX via movsx, glyph chain->ESI via
   `movzx esi,[c]`) flips to match once `y2` is `unsigned char`. The width/a7
   and glyph-advance sub-roles matched after re-spelling the additions in the
   compiler's own evaluation order: `(font[..]+xpos)+a7`, `glyph[4]+a7`, and
   `(a5+j)*6` (a5 first) — all commutative, all byte-for-byte closer.

   Remaining gap (~all reloc-shifted jumps + one real idiom): the 4a6c8 y-arg
   widen is still and-form (mov ax,dx; and eax,0xffff, +3 bytes) vs target
   xor-form (xor eax,eax; mov ax,dx); the value has genuinely-dirty upper bits
   (y param dword-slot load) so both must mask — pure instruction-selection
   tie, resisted every ypos re-spelling. Plus two same-length register-idiom
   ties (glyph-index a5/j EAX<->EDX, space-branch a5 ESI vs EDX). Everything
   else matches, incl. the y+=a4 half-widen (y must be `unsigned short`), the
   deferred j store, saved-buffer in EBP, and both font-index computations.

   The UI text drawer. Walks the NUL-terminated string s one byte at
   a time (byte index i). '\n' resets the pen x to the x param and advances y by
   a4 (the line height). For every other char the glyph record is the 6-byte
   entry tbl + (c - 0x20 + a5) * 6 (a5 = font/colour bank offset; +4 = advance
   width, +5 = height). If a9 (underline colour) is nonzero, first draws a line
   via FUN_18ae8 from (x, y+a4-2) to (x + width + a7, y+a4-2) in colour a9.
   Space (c == 0x20) just advances the pen by the bank-0 glyph width + a7
   (a7 = per-char kerning, -2 at most call sites). Otherwise blits the glyph
   with FUN_4a6c8 at (x, y + 12 - height), temporarily swapping the back-buffer
   pointer g_screen_buf to g_back_buf around the blit when a10 is set. a8 is unused. */
extern unsigned char *g_screen_buf;
extern unsigned char *g_back_buf;

extern void draw_line(int x1, int y1, int x2, int y2, int c);
extern void draw_sprite_rle_buf(int x, int y, unsigned char *spr);

void draw_ui_text(char *s, unsigned short x, unsigned short y, unsigned char a4,
                  unsigned short a5, unsigned char *tbl, signed char a7,
                  signed char a8, unsigned char a9, unsigned char a10)
{
    unsigned char j;
    unsigned char i;
    unsigned short xpos;
    unsigned int ypos;
    unsigned char *saved;
    unsigned char *font;
    unsigned char *glyph;
    unsigned char y2;

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
            draw_line((short)xpos, (short)(a4 + y - 2),
                         (short)((font[(a5 + (unsigned)j - 0x20) * 6 + 4] + xpos) + a7),
                         (short)(a4 + y - 2), a9);
        }
        if (j == 0)
            goto done;
        j -= 0x20;
        if (j != 0) {
            if (a10) {
                saved = g_screen_buf;
                g_screen_buf = g_back_buf;
            }
            glyph = font + (a5 + j) * 6;
            y2 = glyph[5];
            ypos = (unsigned short)(y + 0xc - y2);
            draw_sprite_rle_buf(xpos, ypos, glyph);
            xpos += glyph[4] + a7;
            if (a10)
                g_screen_buf = saved;
        } else {
            xpos += font[a5 * 6 + 4] + a7;
        }
    }
next:
    ++i;
    goto top;
done:;
}
