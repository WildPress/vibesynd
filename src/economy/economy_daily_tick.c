/* @ 0x15f58 -- daily/time economy tick.  Reads 5 volatile input-edge flags
   (g_e2bf..g_e2c3) that toggle bookkeeping bytes / nudge the game-speed global
   g_5304 (busy-wait while the flag==1).  Then, for the normal path
   (g_radar_detail==0): busy-waits on g_10b50 vs speed, recomputes a displayed
   money/rate byte (g_3ee8) and commits the funding entry via FUN_33568 when the
   status g_5594 is 0 or 3, and if the record's time-budget dword can cover
   (param_1-1) days, spends them: money-=days, day++ (wrap at 0x16d -> year++),
   then runs the 50-region economic sweep (FUN_16318 owner funding, g_player_recs +=
   FUN_16438 income, g_syndicate_money[i] += 0x1f4-rand) and FUN_164c8 target reassignment.
   The fast path (g_radar_detail!=0) just spends the days with no sweep.  Overflow
   (not enough budget) advances by param_1/0x60 or 1 depending on g_10b52.
   Record stride 0x417 based at g_player_recs(accum)/g_player_budget(budget,day,year), indexed
   by g_cur_player, exactly as siblings FUN_16318/16438/164c8/16678/33568.
   Recipe: -4s -oneatx -zp8 -s -zq

   PARKED near-miss: ours 939B vs target 957B, difflib 0.571 (541/957),
   structure byte-faithful. Levers that landed: per-block `int idx=g_cur_player*0x417`
   (folds g_player_budget into disp32, killed a 1381B recompute blowup); `g_5304 > 0`
   (JBE not JE); named `money` load; volatile edge-flags + g_cur_player + g_10b50.
   WALL (entry-scheduler, same class as sibling 0x16318's residue): g_5304 is a
   global register-cached in EDI across the whole fn (spill-before-call, reload-
   after). After the FUN_33568 call our reload `mov edi,[g_5304]` schedules EARLY
   (before the idx/money loads); the target schedules it LATE (after the money
   load). That 2-instruction swap flips money's home register (ours ECX `8b8b`,
   target EBP `8bab`), which rewrites nearly every modrm byte in the whole back
   half (main + block2), collapsing the score. Also the `g_10b50=0` zero-reg is
   DH (ours) vs AH (target) -- an encoding tie. Not source-reachable: function-
   scope money decl, `-or` (way off, first diff 0x8), and `>` vs `!=` spellings
   don't move the reload schedule. */
extern volatile unsigned char g_e2bf;
extern volatile unsigned char g_e2c0;
extern volatile unsigned char g_e2c1;
extern volatile unsigned char g_e2c2;
extern volatile unsigned char g_e2c3;
extern unsigned char g_537f;
extern unsigned char g_10b4d;
extern unsigned char g_offscreen_obj;
extern unsigned int g_5304;
extern volatile unsigned char g_10b50;
extern unsigned char g_radar_detail;
extern unsigned char g_10b52;
extern int g_10b06;
extern volatile short g_cur_player;
extern unsigned char g_player_budget[];
extern unsigned char g_player_recs[];
extern unsigned char g_3ee8;
extern unsigned char g_5594;
extern unsigned char g_syndicate_money[];
extern unsigned char commit_funding(void);
extern void FUN_00016318(unsigned char i);
extern unsigned int FUN_00016438(unsigned char i);
extern void FUN_000164c8(void);
extern unsigned short lcg_rand(unsigned short n);

char economy_daily_tick(unsigned int param_1)
{
    unsigned int money;
    char ret;
    unsigned char i;

    ret = 0;
    if (g_e2bf != 0) {
        while (g_e2bf == 1);
        g_537f = (g_537f + 1) & 1;
    }
    if (g_e2c0 != 0) {
        while (g_e2c0 == 1);
        g_10b4d = (g_10b4d + 1) & 1;
    }
    if (g_e2c1 != 0 && g_5304 > 0) {
        while (g_e2c1 == 1);
        g_5304--;
    }
    if (g_e2c2 != 0 && g_5304 < 0xc) {
        while (g_e2c2 == 1);
        g_5304++;
    }
    if (g_e2c3 != 0) {
        while (g_e2c3 == 1);
        g_offscreen_obj = (g_offscreen_obj + 1) & 1;
    }
    if (g_radar_detail == 0) {
        if (g_5304 >= 3) {
            while (g_10b50 < g_5304 && g_10b50 != 0);
        }
        g_10b50 = 0;
        {
            int idx = g_cur_player * 0x417;
            unsigned char v = *(unsigned int *)(g_player_budget + idx) / (param_1 / 0x18);
            if (v != g_3ee8) {
                unsigned char sst = g_5594;
                g_3ee8 = v;
                if (sst == 0 || sst == 3)
                    g_5594 = commit_funding();
            }
        }
        {
            int idx = g_cur_player * 0x417;
            money = *(unsigned int *)(g_player_budget + idx);
            if (param_1 - 1 <= money) {
                ret = 1;
                *(unsigned int *)(g_player_budget + idx) = money - (param_1 - 1);
                *(unsigned short *)(g_player_budget + idx + 4) += 1;
                if (*(unsigned short *)(g_player_budget + idx + 4) > 0x16d) {
                    *(unsigned short *)(g_player_budget + idx + 6) += 1;
                    *(unsigned short *)(g_player_budget + idx + 4) = 1;
                }
                for (i = 0; i < 0x32; i++) {
                    FUN_00016318(i);
                    *(int *)(g_player_recs + g_cur_player * 0x417) += FUN_00016438(i);
                    *(int *)(g_syndicate_money + i * 10) += 0x1f4 - (lcg_rand(0x3e8) & 0xffff);
                }
                FUN_000164c8();
            } else {
                if (g_10b52 != 0) {
                    g_10b06 += param_1 / 0x60;
                    *(unsigned int *)(g_player_budget + idx) = money + param_1 / 0x60;
                } else {
                    *(unsigned int *)(g_player_budget + idx) = money + 1;
                    g_10b06 += 1;
                }
            }
        }
    } else {
        int idx = g_cur_player * 0x417;
        money = *(unsigned int *)(g_player_budget + idx);
        if (param_1 <= money) {
            ret = 1;
            *(unsigned int *)(g_player_budget + idx) = money - param_1;
            *(unsigned short *)(g_player_budget + idx + 4) += 1;
            if (*(unsigned short *)(g_player_budget + idx + 4) > 0x16d) {
                *(unsigned short *)(g_player_budget + idx + 6) += 1;
                *(unsigned short *)(g_player_budget + idx + 4) = 1;
            }
        } else {
            if (g_10b52 != 0) {
                *(unsigned int *)(g_player_budget + idx) += param_1 / 0x60;
                g_10b06 += param_1 / 0x60;
            } else {
                *(unsigned int *)(g_player_budget + idx) = money + 1;
                g_10b06 += 1;
            }
        }
    }
    return ret;
}
