/* frameless @ 0x36648: text-width / kerning accumulator. For each char of str
   (until 0 or '\n'): idx = (arg3 + c - 0x20); w = table[idx].w (6-byte records,
   width byte at +4); acc += w + arg4; return acc. arg3 u16, arg4 signed char.

   NEAR-MISS (dist=4, NOT matched). Register-ROLE + extension-width wall: target puts
   the glyph width in the accumulator (al -> xor ah,ah) and sign-extends arg4 into the
   16-bit bx (`movsx bx`), then `add eax,ebx`; our Watcom puts arg4 in eax (32-bit
   `movsx eax`) and width in bx. Every addition operand-order form (w+arg4, arg4+w,
   temp, assoc, split statements) either keeps our role or grows the code to 86B (uses
   `and eax,0xffff` instead of the compact `movsx bx`). The target's mixed 16-bit-movsx
   / accumulator layout is not source-reachable with 9.5b. Encoding-tie-break wall. */

struct glyph { char a; char b; char c; char d; unsigned char w; char e; };

int FUN_00036648(unsigned char *str, struct glyph *table, int param_3, signed char param_4)
{
    int acc = 0;
    while (*str != 0 && *str != 0x0a) {
        int idx = (unsigned short)param_3 + *str - 0x20;
        acc += param_4 + (unsigned short)table[idx].w;
        str++;
    }
    return acc;
}
