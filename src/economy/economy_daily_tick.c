/* @ 0x15f58 -- daily/time economy tick.  Reads 5 volatile input-edge flags
   (g_e2bf..g_e2c3) that toggle bookkeeping bytes / nudge the game-speed global
   g_game_speed (busy-wait while the flag==1).  Then, for the normal path
   (g_radar_detail==0): busy-waits on g_timer_tick vs speed, recomputes a displayed
   money/rate byte (g_3ee8) and commits the funding entry via FUN_33568 when the
   status g_fund_block is 0 or 3, and if the record's time-budget dword can cover
   (param_1-1) days, spends them: money-=days, day++ (wrap at 0x16d -> year++),
   then runs the 50-region economic sweep (FUN_16318 owner funding, g_player_recs +=
   FUN_16438 income, g_syndicate_money[i] += 0x1f4-rand) and FUN_164c8 target reassignment.
   The fast path (g_radar_detail!=0) just spends the days with no sweep.  Overflow
   (not enough budget) advances by param_1/0x60 or 1 depending on g_10b52.
   Record stride 0x417 based at g_player_recs(accum)/g_player_budget(budget,day,year), indexed
   by g_cur_player, exactly as siblings FUN_16318/16438/164c8/16678/33568.
   Recipe: -4s -oneatx -zp8 -s -zq

   PARKED near-miss: ours 939B vs target 957B, EDIT-DIST=322 (~66% by
   1-editdist/maxlen), structure byte-faithful. Levers that landed: per-block
   `int idx=g_cur_player*0x417` (folds g_player_budget into disp32, killed a
   1381B recompute blowup); `g_game_speed > 0` (JBE not JE); named `money` load;
   volatile edge-flags + g_timer_tick.
   KEY FIX (336 -> 322): g_cur_player is a PLAIN global, not volatile. Watcom
   still reloads it per basic block (movsx at 0xd3/0x138/0x1cc/0x2bc -- it never
   caches a global across calls), so dropping `volatile` costs no reloads but
   lets the g_game_speed resync move LATE. g_game_speed is register-cached in EDI across the
   whole fn (spill-before-call at 0x110, reload-after). With volatile g_cur_player
   the reload `mov edi,[g_game_speed]` scheduled EARLY (0x138, before idx/money); plain
   global lets it schedule LATE (0x161, after the money load) -- matching target
   exactly and also realigning the loop's `add esp,4` (0x1e3).
   WALL (Watcom-9.5 codegen seed, not source-reachable): a global 3-register
   ROTATION {ebp<-ecx<-edx<-ebp}. Ours puts money in ECX where target uses EBP
   (main path) / EDX (block2); this forces the loop's g_player_recs temp to EBP
   (target EDX) and the g_syndicate_money temp to EDX (target ECX, which also
   adds a stray `mov eax,edx`). It rewrites nearly every modrm byte in the back
   half. Confirmed global not loop-driven: block2 has zero loop pressure yet
   shows the identical money ECX-vs-EDX rotation. Also `g_timer_tick=0` zero-reg is
   DH (ours) vs AH (target) and `ret=1` reg CH/DL/AH -- encoding ties. Not moved
   by: register hint on money, delta/need temps, function-scope money decl,
   `-or`, `>` vs `!=`. */
extern volatile unsigned char g_e2bf;
extern volatile unsigned char g_e2c0;
extern volatile unsigned char g_e2c1;
extern volatile unsigned char g_e2c2;
extern volatile unsigned char g_e2c3;
extern unsigned char g_snd_slot_gate;
extern unsigned char g_10b4d;
extern unsigned char g_offscreen_obj;
extern unsigned int g_game_speed;
extern volatile unsigned char g_timer_tick;
extern unsigned char g_radar_detail;
extern unsigned char g_10b52;
extern int g_10b06;
extern short g_cur_player;
extern unsigned char g_player_budget[];
extern unsigned char g_player_recs[];
extern unsigned char g_3ee8;
extern unsigned char g_fund_block;
extern unsigned char g_syndicate_money[];
extern unsigned char commit_funding(void);
extern void research_funding_tick(unsigned char i);
extern unsigned int syndicate_timer_pct(unsigned char i);
extern void player_target_sweep(void);
extern unsigned short lcg_rand(unsigned short n);

char economy_daily_tick(unsigned int param_1)
{
    unsigned int money;
    char ret;
    unsigned char i;

    ret = 0;
    if (g_e2bf != 0) {
        while (g_e2bf == 1);
        g_snd_slot_gate = (g_snd_slot_gate + 1) & 1;
    }
    if (g_e2c0 != 0) {
        while (g_e2c0 == 1);
        g_10b4d = (g_10b4d + 1) & 1;
    }
    if (g_e2c1 != 0 && g_game_speed > 0) {
        while (g_e2c1 == 1);
        g_game_speed--;
    }
    if (g_e2c2 != 0 && g_game_speed < 0xc) {
        while (g_e2c2 == 1);
        g_game_speed++;
    }
    if (g_e2c3 != 0) {
        while (g_e2c3 == 1);
        g_offscreen_obj = (g_offscreen_obj + 1) & 1;
    }
    if (g_radar_detail == 0) {
        if (g_game_speed >= 3) {
            while (g_timer_tick < g_game_speed && g_timer_tick != 0);
        }
        g_timer_tick = 0;
        {
            int idx = g_cur_player * 0x417;
            unsigned char v = *(unsigned int *)(g_player_budget + idx) / (param_1 / 0x18);
            if (v != g_3ee8) {
                unsigned char sst = g_fund_block;
                g_3ee8 = v;
                if (sst == 0 || sst == 3)
                    g_fund_block = commit_funding();
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
                    research_funding_tick(i);
                    *(int *)(g_player_recs + g_cur_player * 0x417) += syndicate_timer_pct(i);
                    *(int *)(g_syndicate_money + i * 10) += 0x1f4 - (lcg_rand(0x3e8) & 0xffff);
                }
                player_target_sweep();
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
