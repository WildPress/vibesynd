/* frameless @ 0x338d8: table save/restore state machine. TRUE SIZE 418 (manifest
   said 295 — undercount past Ghidra's carve gap at 0x33990-0x33a11, recovered via
   disassemble_bytes; manifest fixed).
   Sets g_fund_block=2, then switch(g_fund_mode): case 1 copies the 10x24 short table g_fund_grid
   into bank g_fund_bank of the array at 0x578a (stride 0x1eb); case 2 copies it into
   bank g_fund_bank of the array at 0x7bf5 (stride 0x1f5, table at +0x12) and abs()es
   the bank's leading int each iteration. Then always: clears g_fund_mode/g_fund_bank, sets
   g_fund_sel=-1, g_fund_qty=0, and zeroes g_fund_grid.

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
extern unsigned char g_fund_block;
extern unsigned char g_fund_mode;
extern unsigned char g_fund_bank;
extern int g_fund_sel;
extern short g_fund_qty;
extern short g_fund_grid[10][24];
extern unsigned char g_578a[];
extern unsigned char g_7bf5[];

void table_save_restore(void)
{
    g_fund_block = 2;
    switch (g_fund_mode) {
    case 1: {
        unsigned char r, c;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++)
                *(short *)(g_578a + g_fund_bank * 0x1eb + r * 48 + c * 2) = g_fund_grid[r][c];
        break;
    }
    case 2: {
        unsigned char c, r;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++) {
                *(short *)(g_7bf5 + g_fund_bank * 0x1f5 + 0x12 + r * 48 + c * 2) = g_fund_grid[r][c];
                if (*(int *)(g_7bf5 + g_fund_bank * 0x1f5) < 0)
                    *(int *)(g_7bf5 + g_fund_bank * 0x1f5) = -*(int *)(g_7bf5 + g_fund_bank * 0x1f5);
            }
        break;
    }
    }
    g_fund_mode = 0;
    g_fund_bank = 0;
    g_fund_sel = -1;
    g_fund_qty = 0;
    {
        unsigned char r, c;
        for (r = 0; r < 10; r++)
            for (c = 0; c < 24; c++)
                g_fund_grid[r][c] = 0;
    }
}
