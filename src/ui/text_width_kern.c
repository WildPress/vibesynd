/* frameless @ 0x36648: text-width / kerning accumulator. For each char of str
   (until 0 or '\n'): idx = (arg3 + c - 0x20); w = table[idx].w (6-byte records,
   width byte at +4); acc += w + arg4; return acc. arg3 u16, arg4 signed char.

   NEAR-MISS (raw-diff ~12B, byte-identical for the first 51 of 79 bytes; NOT matched).
   cont.32 CLOSED ground the earlier header called unreachable. Typing the glyph
   width as a 16-bit local (`short w = (unsigned short)table[idx].w`) flips the whole
   register role to the target's: width lands in the accumulator (eax) and arg4 goes to
   ebx, and the length drops from 83B to the target's 79B. Everything up to offset 0x33
   is now byte-identical. Prior operand-order/temp/assoc forms all kept arg4 in eax and
   width in bx (the wrong role); the type-directed lever was the missing one.

   What remains is one 12-byte block, two coupled ties:
     (1) load schedule -- target loads arg4 (`movsx bx`) before the width byte; ours
         loads width first. (2) widen encoding -- target `movsx bx` (16-bit, 66-prefix)
         with no width extension (it proves the accumulator's upper 16 bits are already
         clean); ours `movsx ebx` (32-bit) plus a `cwde` on the short. Both are the same
         12 bytes reordered. Every attempt to force the 16-bit widen (cast arg4 to
         (short), retype it, reorder the add) ripples the whole allocation and pushes the
         first diff back to 0x16 -- the interference graph pins the widen to the loop
         guard. 16/32 widen + schedule tie-break wall. */

struct glyph { char a; char b; char c; char d; unsigned char w; char e; };

int text_width_kern(unsigned char *str, struct glyph *table, int param_3, signed char param_4)
{
    int acc = 0;
    while (*str != 0 && *str != 0x0a) {
        int idx = (unsigned short)param_3 + *str - 0x20;
        { short w = (unsigned short)table[idx].w; acc += w + param_4; }
        str++;
    }
    return acc;
}
