/* MATCHED (RELOC-AWARE, dist 2 = the two masked g_e11c reloc bytes). Was a parked
   near-miss at dist 53 (register-role wall). Two fixes cracked it:
     1. `volatile int param_3` -- the target computes s by destroying param_3 in place
        (sub in-place) then RELOADS the pristine stack home for the +0x14 read in state 3;
        volatile models that exact reload and collapses the frame from 3 callee-saved
        pushes (edi) down to 2 (ebx,esi).
     2. `int s` (not `short s`) -- flips Watcom's register colouring so param_1 lands in
        ebx and s in edx (matching the target); a `short` local instead grabbed the
        callee-saved ebx for s and pushed param_1 into a scratch reg, cascading into a
        tail-merge and a bx-reuse for the g_e11c store. The (short) casts at the compares
        keep the 16-bit `cmp dx,...`. */
/* frameless @ 0x2d038: record-state machine. (short)(param_3 - 0x810e) is the pool id
   of node param_3; compared to *(short*)(param_1+0x44). Advances state at param_2
   (+0=state, +0xa=flag byte, +0x10=stamp) toward 3, else falls back to 1. */
extern short g_e11c;

void FUN_0002d038(unsigned char *param_1, short *param_2, volatile int param_3, short param_4)
{
    int s = param_3 - 0x810e;

    if (*param_2 == 0) {
        if ((short)s == *(short *)(param_1 + 0x44))
            *param_2 = 3;
        else
            *param_2 = 1;
        *(unsigned char *)(param_2 + 5) = 2;
        param_2[8] = param_4;
        return;
    }
    if (*param_2 == 1 && (short)s == *(short *)(param_1 + 0x44)) {
        *param_2 = 3;
        *(unsigned char *)(param_2 + 5) = 2;
        param_2[8] = param_4;
        return;
    }
    if (*param_2 == 3 && *(short *)(param_1 + 0x44) == 0 &&
        *(short *)(param_3 + 0x14) < 0 && *(char *)(param_2 + 5) == 0) {
        *param_2 = 1;
        param_2[8] = param_4;
        *(unsigned char *)(param_2 + 5) = 2;
        g_e11c = 0;
    }
}
