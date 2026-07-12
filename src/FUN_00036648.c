/* frameless @ 0x36648: text-width / kerning accumulator. For each char of str
   (until 0 or '\n'): idx = (arg3 + c - 0x20); w = table[idx].w (6-byte records,
   width byte at +4); acc += w + arg4; return acc. arg3 u16, arg4 signed char. */

struct glyph { char a; char b; char c; char d; unsigned char w; char e; };

int FUN_00036648(unsigned char *str, struct glyph *table, unsigned short param_3, signed char param_4)
{
    int acc = 0;
    while (*str != 0 && *str != 0x0a) {
        int idx = param_3 + *str - 0x20;
        unsigned short d = (unsigned short)table[idx].w + (short)param_4;
        acc += d;
        str++;
    }
    return acc;
}
