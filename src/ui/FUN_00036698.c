/* @ 0x36698: PARKED near-miss 295/362 (aligned bytes, -4s -oneatx -zp8 -s -zq).
   Remaining walls (all §3 families): (1) spill-slot order — target has i at
   [esp+0], c at [esp+4]; ours always swaps them under the for(;;)/goto loop
   form that reproduces the target's test-at-top layout. A real
   `for (i=0; s[i]; ++i)` header DOES give i slot 0 (structured-loop-var rank)
   but then -oneatx converts to jump-to-test layout (entry jmp to a bottom
   test) which the target does not have — the two requirements co-vary in
   opposite directions. (2) underline block register-role: target homes
   y2=(short)(a4+y-2) in EDX and the glyph-index chain in ESI (one-step
   `movzx esi,[c]`); ours picks y2->ESI, chain->EDX (xor+mov byte widen, lea
   -0x20 fold) whatever the operand spelling. (3) the 4a6c8 y-arg widen comes
   out and-form (mov ax,dx; and eax,0xffff) vs target xor-form; full-width-temp
   lever did NOT flip it here. Everything else matches, incl. the y+=a4
   half-widen (y param must be `unsigned short`: dword slot loads with dirty
   upper), the deferred c store (test s[i] in the guards, assign c after the
   newline branch), saved-buffer in EBP, and both font-index computations.

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

extern void FUN_00018ae8(int x1, int y1, int x2, int y2, int c);
extern void FUN_0004a6c8(int x, int y, unsigned char *spr);

void FUN_00036698(char *s, unsigned short x, unsigned short y, unsigned char a4,
                  unsigned short a5, unsigned char *tbl, signed char a7,
                  signed char a8, unsigned char a9, unsigned char a10)
{
    unsigned char i;
    unsigned char j;
    unsigned short xpos;
    unsigned int ypos;
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
                         (short)(a7 + (font[(a5 + (unsigned)j - 0x20) * 6 + 4] + xpos)),
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
            glyph = font + (j + a5) * 6;
            ypos = (unsigned short)(y + 0xc - glyph[5]);
            FUN_0004a6c8(xpos, ypos, glyph);
            xpos += a7 + glyph[4];
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
