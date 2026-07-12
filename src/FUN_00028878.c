/* frameless @ 0x28878: chunked dispatch. Call FUN_000284a8(p1,p2,p3,chunk) for
   param_4/g_df38 whole chunks of g_df38 (16-bit global divisor), advancing p3 by
   g_df38 each time; stop early if a call returns non-zero. Then one final call
   with the remainder param_4 % g_df38. The div is recomputed each iteration
   (the call may change g_df38). Callee's chunk arg is 16-bit (unsigned short). */
extern unsigned short g_df38;
extern short FUN_000284a8(int, unsigned int, unsigned int *, unsigned short);
void FUN_00028878(int param_1, unsigned short param_2, unsigned int *param_3, unsigned int param_4)
{
    unsigned int i = 0;
    goto test;
    do {
        if (FUN_000284a8(param_1, param_2, param_3, g_df38))
            return;
        i++;
        param_3 = (unsigned int *)((int)param_3 + g_df38);
    test:;
    } while (i < param_4 / g_df38);
    FUN_000284a8(param_1, param_2, param_3, param_4 % g_df38);
}
