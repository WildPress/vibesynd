/* frameless @ 0x338d8: table save/restore state machine. TRUE SIZE 418 (manifest
   said 295 — undercount past Ghidra's carve gap at 0x33990-0x33a11, recovered via
   disassemble_bytes; manifest fixed).
   Sets g_5594=2, then switch(g_5595): case 1 copies the 10x24 short table g_559d
   into bank g_5596 of the array at 0x578a (stride 0x1eb); case 2 copies it into
   bank g_5596 of the array at 0x7bf5 (stride 0x1f5, table at +0x12) and abs()es
   the bank's leading int each iteration. Then always: clears g_5595/g_5596, sets
   g_5597=-1, g_559b=0, and zeroes g_559d.

   🅿️ PARKED (structure & all code byte-correct; a single 1-byte encoding ripple):
   SPILL-SLOT-ORDER WALL. The case loops' byte counters spill to [esp+0/4/8/0xc];
   target order is (case1-row@0, case1-col@4, case2-col@8, case2-row@0xc), ours
   always sorts role-first (both rows low, both cols high) whatever the decl
   order/scoping. 10 configs tried: block vs function-top decls, decl order flips
   (provably ignored by Watcom 9.5), reset-loop variable reuse (perturbs col order
   only), case source order (moves code layout, worse). The [esp+0] vs [esp+4]
   disp8 difference shifts one byte and every loop-alignment pad after it. Wall
   class: spill-slot assignment order is allocator-internal, same family as the
   register-role walls. Reset loop's counters stay in BL/BH (registerised). */
extern unsigned char g_5594;
extern unsigned char g_5595;
extern unsigned char g_5596;
extern int g_5597;
extern short g_559b;
extern short g_559d[10][24];
extern unsigned char g_578a[];
extern unsigned char g_7bf5[];

void table_save_restore(void)
{
    g_5594 = 2;
    switch (g_5595) {
    case 1: {
        unsigned char r, c;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++)
                *(short *)(g_578a + g_5596 * 0x1eb + r * 48 + c * 2) = g_559d[r][c];
        break;
    }
    case 2: {
        unsigned char c, r;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++) {
                *(short *)(g_7bf5 + g_5596 * 0x1f5 + 0x12 + r * 48 + c * 2) = g_559d[r][c];
                if (*(int *)(g_7bf5 + g_5596 * 0x1f5) < 0)
                    *(int *)(g_7bf5 + g_5596 * 0x1f5) = -*(int *)(g_7bf5 + g_5596 * 0x1f5);
            }
        break;
    }
    }
    g_5595 = 0;
    g_5596 = 0;
    g_5597 = -1;
    g_559b = 0;
    {
        unsigned char r, c;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++)
                g_559d[r][c] = 0;
    }
}
