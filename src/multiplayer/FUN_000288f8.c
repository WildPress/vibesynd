/* Proposed name: recv_chunked -- the receive counterpart of xfer_chunked (0x28878,
   which is void/send). Receives param_4 bytes in whole chunks of g_df38, verifying
   each chunk: it calls netbios_recv95(p1,p2,p3,0) for param_4/g_df38 whole chunks
   (g_df38 is the 16-bit global chunk size, re-read each iter; callee returns short).
   If a chunk's returned count != g_df38 it stops and returns that count (short read).
   Else it advances the buffer p3 by g_df38. After the whole chunks it does one final
   (remainder) call: if that count == param_4 % g_df38 it returns param_4 (all bytes
   moved), otherwise it returns the short count.

   IMPROVED dist 60->49 by typing the call result `s` as `unsigned int` (was `short`).
   This is semantically identical to `short s`: both sign-extend the 16-bit callee
   return to 32 bits before every compare and before `return s`, so behaviour is
   unchanged -- but `unsigned int` makes the compiler carry a SINGLE sign-extended
   copy (`movsx ecx,ax`) used for both the compare and the shared `out: return s`,
   instead of the short form's redundant save+extend pair (`mov ecx,eax` + `movsx
   edx,ax`). It also coaxes out the target's g_df38-reconstruct idiom in the loop
   body (`xor eax,eax; mov ax,cx` vs target `xor eax,ecx; mov ax,cx` -- one byte
   apart), so the body realigns to ~140 length.

   RESIDUAL WALL (first diff 0x13 = jmp disp, register-role tie). Target keeps the
   call result in EAX (CWDE in place) and loads g_df38 into CX (`66 8b 0d`, 7 bytes);
   ours evicts the result to ECX so AX is free for the 1-byte-shorter `66 a1` moffs
   load of g_df38 -- a genuine cost-model coin flip in the loop BODY (the loop
   CONDITION already forces g_df38 into CX for the DIV, and matches byte-for-byte).
   The tail DIV divisor reg (ECX vs EBX) and the two epilogue-share choices are
   knock-ons. Compare-operand reorder (global-on-left) and int/unsigned-short typings
   all regress. Not source-reachable; needs a register-preference permuter. */
extern unsigned short g_df38;
extern short netbios_recv95(int, unsigned int, unsigned int *, unsigned short);
unsigned int FUN_000288f8(int param_1, unsigned short param_2, unsigned int *param_3, unsigned int param_4)
{
    unsigned int i;
    unsigned int s;
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
