/* measure_text_width @ 0x365e8 - measure summed text width of a string.
 * Walks param_1 until NUL or one of ' ' '\n' '\\' '|'; for each char c looks up
 * a 6-byte-stride record table (param_2) at index (c+param_3-0x20), reads its
 * +4 byte, and accumulates byte+param_4. Sibling of 0x36648 (same shape, extra
 * terminator checks). Returns the accumulated int. Stack-calling (-4s).
 *
 * NEAR-MISS / WALL (§3 CSE double-read). The index/table math (lea ebx*4-sub /
 * [esi+eax*2+4]) matches byte-for-byte; volatile param_3 correctly kills the
 * EDI hoist so only ebx+esi are saved. The irreducible diff: the target reads
 * *p TWICE per iteration -- once into BH for the four terminator compares, then
 * a fresh `mov al,[edx]` (with xor eax,eax) for the value -- whereas -oneatx
 * CSEs the two reads into one and keeps the char in AL (`and eax,0xff` reuse),
 * which also lands the check char in AL not BH. First diff at 0x11 (jz disp,
 * from the AL-vs-BH check reg). Every recipe (-oneatx/-or/-ot/-os) is identical;
 * `volatile char*` param_1 defeats the CSE but then reloads *p for all 4 checks
 * (target loads once) and reshapes the guard -- strictly worse. Not source-
 * reachable via CSE; a read-duplication permuter is needed. */
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
                sum += param_2[k * 6 + 4] + param_4;
            }
            param_1++;
        } while (*param_1 != 0);
    }
    return sum;
}
