/* frameless @ 0x20be8: for the current row g_10b16, count the leading nonzero word
   entries (up to 8) in the strided table at 0xe5c3 for column = param_1. Row stride
   1047, column stride 40, entry stride 4 (words). Stops at the first zero entry and
   returns the count. Sibling of 0x20b78 (which indexes the column via g_10aa4).

   NEAR-MISS (101/101, 4 bytes in 2 spots; logic byte-exact otherwise). Two scheduler
   tie-breaks only: (1) target increments count(CH) then i(CL) -> `fe c5 fe c1`; ours
   `fe c1 fe c5` (i then count). (2) target loads the return value (`mov al,ch`) BEFORE
   the g_10b16 write-back, ours after. Both are pure instruction-schedule transpositions
   of independent ops; do-while/for, count/i decl order, increment order, and a return
   temp all leave the emitted schedule identical. Scheduling wall. */
extern short g_10b16;
extern unsigned char g_e5c3[];
unsigned char FUN_00020be8(unsigned char param_1)
{
    short si = g_10b16;
    unsigned char count = 0;
    unsigned char i = 0;
    do {
        if (*(short *)(g_e5c3 + (int)si * 1047 + (int)param_1 * 40 + (int)i * 4) == 0)
            break;
        count++;
        i++;
    } while (i < 8);
    g_10b16 = si;
    return count;
}
