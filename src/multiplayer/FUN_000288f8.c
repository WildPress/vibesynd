/* PARKED near-miss (NOT matched, ~140/140 len, first diff 0x13) -- chunked-dispatch loop,
   sibling of matched 0x28878. Structure byte-exact (short callee return + goto-out shared
   epilogue gave 140/140). Wall: register-role -- target keeps the call result in EAX (CWDE)
   and loads g_df38 into CX; ours evicts result to ECX so AX is free for the 1-byte-shorter
   `66 a1` moffs load of g_df38; tail DIV uses ECX vs EBX as a knock-on. Not source-reachable. */
/* frameless @ 0x288f8: chunked loop. Sibling of 0x28878. Call
   netbios_recv95(p1,p2,p3,0) for param_4/g_df38 whole chunks (g_df38 is the
   16-bit global divisor, recomputed each iter; callee returns short). If a
   chunk's result != g_df38, return it early; else advance p3 by g_df38. After
   the loop do one final call: return param_4 if its result equals
   param_4 % g_df38, otherwise return that result. */
extern unsigned short g_df38;
extern short netbios_recv95(int, unsigned int, unsigned int *, unsigned short);
unsigned int FUN_000288f8(int param_1, unsigned short param_2, unsigned int *param_3, unsigned int param_4)
{
    unsigned int i;
    short s;
    for (i = 0; i < param_4 / g_df38; i++) {
        s = netbios_recv95(param_1, param_2, param_3, 0);
        if (s != g_df38)
            goto out;
        param_3 = (unsigned int *)((int)param_3 + g_df38);
    }
    s = netbios_recv95(param_1, param_2, param_3, 0);
    if (s == param_4 % g_df38)
        return param_4;
out:
    return s;
}
