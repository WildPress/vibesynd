/* @ 0x36808: PARKED near-miss (987B target, -4s -oneatx -zp8 -s -zq).
   Structure is byte-faithful; the whole divergence cascades from a single §3
   register-role tie: `cur` (param5, ESI in target) and `len` (loop counter, EDI
   in target) swap onto ESI/EDI in ours (cur->EDI, len->ESI). font->EBP and
   text->EBX are assigned IDENTICALLY in both, so only the two index registers
   tie-break the wrong way. The swap is inert to encoding size but cascades into
   scratch-reg picks (BH vs CH zero) and store scheduling (Enter case reorders
   *active/g_e2a0), which is why the back half looks noisy. Levers tried, all
   failed to flip ESI/EDI: named-param-copy (cont.21) only reordered the entry
   param loads; decl-order demotion of len was inert; pointer-diff len (len =
   end-text) spilled len to a slot and grew the frame to 0x60. First diff 0x8
   (8b b4 -> 8b bc). ESI<->EDI tie-break, not source-reachable.

   The keyboard line-editor widget (text-input field).
   Clamps the cursor to the string length, fetches a key (0x4d442) and inserts
   printable chars at the cursor with a right-shift (optionally upper-casing a-z
   when caps set), then dispatches the special-key scancode in g_537e:
   Enter(0x1c) clears active; Home(0x47) cursor=0; End(0x4f) cursor=len;
   Left(0x4b)/Right(0x4d) move; Delete(0x53)/Backspace(0xe) remove (g_e2a1 flag
   switches between single-char and to-end/to-start bulk removal). Each case also
   clears its own repaint-flag byte. Returns 1 when a recognised edit happened,
   else 0. Renders the horizontally-scrolled visible window (width chars) via the
   0x36698 text drawer, then when active draws the cursor as a coloured underline
   (0xfc, or 0x58 when flags&4) spanning the char at the cursor via 0x18ae8. */
extern unsigned char g_537e;
extern unsigned char g_e2a0;
extern unsigned char g_e2a1;
extern unsigned char g_e2cb;
extern unsigned char g_e2cf;
extern unsigned char g_e2d1;
extern unsigned char g_e2d3;
extern unsigned char g_e2d7;
extern unsigned char g_e292;

extern unsigned char FUN_0004d442(void);
extern void FUN_00036698(char *s, unsigned short x, unsigned short y, unsigned char a4,
                         unsigned short a5, unsigned char *tbl, signed char a7,
                         unsigned char a8, unsigned char a9, unsigned char a10);
extern void FUN_00018ae8(int x1, int y1, int x2, int y2, int c);

int FUN_00036808(unsigned short x, unsigned short y, int width, unsigned short maxlen,
                 unsigned short *cur, unsigned short *active, char *text,
                 unsigned char *font, unsigned short colour, unsigned char caps,
                 unsigned char lineht, unsigned char flags, signed char kern,
                 unsigned char a8)
{
    char buf[84];
    unsigned short j;
    int ret;
    char *end;
    char *p;
    char *q;
    int len;
    unsigned char key;
    unsigned char ch;
    short d;
    unsigned short k;
    int cx;

    end = text;
    len = 0;
    if (*end != 0) {
        do { ++end; ++len; } while (*end != 0);
    }
    if ((unsigned short)len < *cur)
        *cur = (unsigned short)len;

    ret = 0;
    if (*active != 0) {
        key = FUN_0004d442();
        ch = key;
        if (key != 0 && key >= 0x20 && (unsigned short)len < maxlen) {
            if (key >= 0x61 && key <= 0x7a && caps != 0)
                ch -= 0x20;
            p = text + *cur;
            q = end;
            while (q >= p) { --q; q[2] = q[1]; }
            *p = ch;
            ++*cur;
        }
        ret = 1;
        switch (g_537e) {
        case 0x1c:  /* Enter */
            *active = 0;
            g_e2a0 = 0;
            break;
        case 0x47:  /* Home */
            *cur = 0;
            g_e2cb = 0;
            break;
        case 0x4f:  /* End */
            *cur = (unsigned short)len;
            g_e2d3 = 0;
            break;
        case 0x4b:  /* Left */
            if (*cur != 0)
                *cur = *cur - 1;
            g_e2cf = 0;
            break;
        case 0x4d:  /* Right */
            if ((unsigned short)len > *cur)
                *cur = *cur + 1;
            g_e2d1 = 0;
            break;
        case 0x53:  /* Delete */
            if ((unsigned short)len > *cur) {
                if (g_e2a1 != 0) {
                    text[*cur] = 0;
                } else {
                    p = text + *cur;
                    if (*p != 0) {
                        do { p[0] = p[1]; ++p; } while (*p != 0);
                    }
                }
            }
            g_e2d7 = 0;
            break;
        case 0xe:   /* Backspace */
            if (*cur != 0) {
                if (g_e2a1 != 0) {
                    p = text + *cur - 1;
                    q = text;
                    if (*p != 0) {
                        do { *q = p[1]; ++p; ++q; } while (*p != 0);
                    }
                    *cur = 0;
                } else {
                    p = text + *cur - 1;
                    if (*p != 0) {
                        do { p[0] = p[1]; ++p; } while (*p != 0);
                    }
                    --*cur;
                }
            }
            g_e292 = 0;
            break;
        default:
            ret = 0;
            break;
        }
    }
    g_537e = 0;

    d = *cur - width;
    if (d < 0)
        d = 0;
    p = text + d;
    j = 0;
    if (*p != 0) {
        while (j < (unsigned short)width) {
            buf[j] = *p;
            ++p;
            ++j;
            if (*p == 0)
                break;
        }
    }
    buf[j] = 0;

    j = 0;
    if (*active != 0) {
        j = 0xfc;
        if (flags & 4)
            j = 0x58;
    }

    FUN_00036698(buf, x, y, lineht, colour, font, kern, a8, 0, 0);

    if (*active != 0) {
        cx = 0;
        k = 0;
        if (*cur != 0) {
            do {
                cx += kern + font[(colour + (unsigned char)buf[k] - 0x20) * 6 + 4];
                ++k;
            } while (k != *cur);
        }
        cx += x;
        FUN_00018ae8((short)cx, y + lineht - 2,
                     cx + font[(colour + (unsigned char)buf[*cur] - 0x20) * 6 + 4] + kern,
                     y + lineht - 2, (unsigned char)j);
    }

    return ret;
}
