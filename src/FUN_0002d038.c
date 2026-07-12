/* PARKED near-miss (NOT matched, fuzzer 117/144, 8000 variants) -- branchy state machine
   on param_2. Structure correct; register-role wall. Agent + max-worker cpermute both plateau. */
/* frameless @ 0x2d038: record-state machine. (short)(param_3 - 0x810e) is the pool id
   of node param_3; compared to *(short*)(param_1+0x44). Advances state at param_2
   (+0=state, +0xa=flag byte, +0x10=stamp) toward 3, else falls back to 1. */
extern short g_e11c;

void FUN_0002d038(unsigned char *param_1, short *param_2, int param_3, short param_4)
{
    short s = (short)(param_3 - 0x810e);

    if (*param_2 == 0) {
        if (s == *(short *)(param_1 + 0x44))
            *param_2 = 3;
        else
            *param_2 = 1;
        *(unsigned char *)(param_2 + 5) = 2;
        param_2[8] = param_4;
        return;
    }
    if (*param_2 == 1 && s == *(short *)(param_1 + 0x44)) {
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
