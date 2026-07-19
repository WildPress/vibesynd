/* measure_text_width @ 0x365e8 - measure summed text width of a string.
 * Walks param_1 until NUL or one of ' ' '\n' '\\' '|'; for each char c looks up
 * a 6-byte-stride record table (param_2) at index (c+param_3-0x20), reads its
 * +4 byte, and accumulates byte+param_4. Sibling of 0x36648 (same shape, extra
 * terminator checks). Returns the accumulated int. Stack-calling (-4s).
 *
 * IMPROVED dist 60->42->30 (match ~54%->~67%). The type-directed lever from the
 * matched sibling 0x36648 applies here too: typing the glyph width as a 16-bit
 * local (`short w = (unsigned short)table[..]`) flips the register roles to the
 * target's -- width lands in EBX and param_4 in EAX (`add eax,ebx`) -- and drops
 * the frame. The index/table math (lea ebx*4-sub / [esi+eax*2+4]) already matched.
 *
 * RESIDUAL WALL (first diff 0x11, CSE double-read). The target reads *p TWICE per
 * iteration -- once into BH for the four terminator compares (3-byte `cmp bh,imm`),
 * then a fresh `mov al,[edx]` for the value -- whereas Watcom CSEs our two reads
 * into one and keeps the char in AL (2-byte `cmp al,imm`), which also lands the
 * check char in AL not BH and reorders the param_3/char load. A `volatile`-
 * qualified value read *does* split the load and restores the target's mid-loop
 * param_3-in-BX / fresh-char-in-AL roles (0x28-0x33 region), but nets +1B (dist
 * 31) and cannot move the AL-vs-BH check reg -- strictly worse. Signed-vs-unsigned
 * char reads do not defeat the CSE. Two coupled ties remain: (1) AL-vs-BH check
 * reg from the CSE'd load; (2) param_4 widen `movsx ax` 16-bit (target proves the
 * accumulator's upper 16 clean) vs our `movsx e-x` 32-bit. Not source-reachable. */
int measure_text_width(char *param_1, unsigned char *param_2,
                 volatile unsigned short param_3, signed char param_4)
{
    int sum = 0;
    if (*param_1 != 0) {
        do {
            unsigned char c = *param_1;
            if (c == ' ' || c == '\n' || c == '\\' || c == '|')
                break;
            {
                int k = (unsigned char)*param_1 + param_3 - 0x20;
                short w = (unsigned short)param_2[k * 6 + 4];
                sum += w + param_4;
            }
            param_1++;
        } while (*param_1 != 0);
    }
    return sum;
}
