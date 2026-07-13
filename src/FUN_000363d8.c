/* @ 0x363d8 (523 bytes): word-wrap text layout/draw engine.
   Draws string s glyph-by-glyph with `font` (6-byte glyph records: width at +4,
   height at +5), word-wrapping inside the box (x0,y0,w,h). a7 = glyph base offset
   added to each char code, a8 = line height (low byte used), a9 = signed char
   spacing, a10 unused, out = (optional) receives the remaining-string pointer on
   vertical overflow. Words are measured with FUN_000365e8; each glyph is drawn at
   (v.x, v.y+12-glyphheight) via FUN_0004a6c8. Separators: ' ' 0x0a 0x5c('\') 0x7c('|');
   "\n\n" = blank line (v.y += 2*a8, v.x=x0), '\' = newline, '|' = terminator, TAB is
   rewritten to space in the buffer. Returns 0 on overflow (out set), 1 otherwise. */

extern int FUN_000365e8(char *s, unsigned char *font, int base, int spacing);
extern void FUN_0004a6c8(int x, int y, unsigned char *glyph);

char FUN_000363d8(char *s, int x0, int y0, int w, int h,
                  unsigned char *font, int a7, int a8, int a9,
                  int a10, char **out)
{
    unsigned char *fp = font;
    char **op = out;
    struct {
        int y2;
        int y;
        int x;
    } v;
    int ww;
    int t;
    char r;

    v.x = x0;
    v.y = y0;
    while (*s != 0 && *s != 0x7c) {
        ww = FUN_000365e8(s, fp, (unsigned short)a7, (signed char)a9);
        if ((unsigned short)ww + (t = (unsigned short)v.x) > (unsigned short)x0 + (unsigned short)w) {
            v.x = (unsigned short)x0;
            v.y += (unsigned char)a8;
        }
        if ((unsigned short)y0 + (unsigned short)h < (t = (unsigned short)v.y)) {
            if (op) {
                *op = s;
            }
            r = 0;
            goto done;
        }
        v.y2 = t + 0xc;
        while (*s != 0 && *s != 0x20 && *s != 0xa && *s != 92 && *s != 0x7c) {
            if (*s == 9) {
                *s = 0x20;
            }
            FUN_0004a6c8((unsigned short)v.x,
                         (unsigned short)(v.y2 - *(fp + ((unsigned short)a7 + *s - 0x20) * 6 + 5)),
                         fp + (*s - 0x20 + (unsigned short)a7) * 6);
            v.x += (unsigned short)(fp[(*s + (unsigned short)a7 - 0x20) * 6 + 4] + (signed char)a9);
            s++;
        }
        v.x += (unsigned short)fp[(unsigned short)a7 * 6 + 4];
        do {
            if (*s != 0x20 && *s != 0xa && *s != 92 && *s != 0x7c) {
                break;
            }
            if (*s == 0xa && *s == s[1]) {
                v.y += (unsigned short)((unsigned char)a8 * 2);
                v.x = x0;
            }
            if (*s == 92) {
                v.y += (unsigned short)(unsigned char)a8;
                v.x = x0;
            }
            if (*s == 0x7c) {
                break;
            }
            s++;
        } while (*s != 0);
    }
    r = op ? 1 : 1;
done:
    return r;
}
