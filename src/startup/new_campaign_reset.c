/* new_campaign_reset @ 0x20fc8 -- TRUE SIZE 1675 (0x20fc8..0x21652 inclusive,
 * ends with the ret at 0x21652; next fn new_game_reset at 0x21658).
 *
 * WARNING -- STALE MANIFEST SIZE. manifest/functions.json records size 1510 for
 * this function, which is WRONG. 0x20fc8 + 1510 = 0x215ae lands mid-way through
 * the mod-record loop, so the harness (tools/match95.sh -> tools/match_reloc.py)
 * slices the target at 1510 bytes and compares our full body against a truncated
 * target. That truncation is the sole source of the reported "ours is +117
 * bytes LONGER". Against the real 1675-byte target our -4s body is 1627 bytes,
 * i.e. ~48 bytes SHORTER, and 436 instructions vs the target's 440. There is no
 * +117 reconstruction gap. (functions.json is owned elsewhere; not edited here.)
 *
 * RECIPE -- use -4s (stack calling), NOT the -4r in manifest/equivalence.json.
 * The real target passes every call argument on the stack: lcg_rand(3/7/0x14)
 * as `push imm; call; add esp,4`, and fill_bytes as `push 0x1e9; push 0;
 * push &g_5594; call; add esp,0xc`. -4r makes our calls register-pass and
 * diverge at every call site; -4s reproduces the prologue (push ebx/esi/edi/ebp)
 * and all five call sites byte-for-byte in shape.
 *
 * "Start a new campaign" -- resets every persistent per-player / per-syndicate
 * table the strategy layer owns. Called once when a new game begins. It walks
 * a handful of fixed-address record tables and stamps their starting values.
 *
 * STATUS: reconstructed, and against the CORRECT 1675-byte target this is a
 * close, tie-dominated match, not a structural gap. Under -4s the prologue and
 * all five call sites match; the residue is the Watcom-9.5 codegen wall
 * (register allocation, immediate-vs-register constant forms, loop-counter
 * memory-homing and stack-slot colouring, loop-alignment NOPs). The logic and
 * store layout are byte-faithful; what still differs is which register/encoding
 * the compiler picked per site, and the target homes two more dword spill slots
 * (sub esp,0x18 vs our sub esp,0x10), so it is if anything slightly bulkier
 * than ours -- there are no extra ops to remove on our side.
 *
 * Structural fixes that moved the needle:
 *   1. The research (0x5788) and mod (0x7bf4) records: the target DUPLICATES the
 *      10x24-word zeroing sub-loop into BOTH the unlimited-funds and normal
 *      branches (not one shared loop after the if). Source now mirrors that.
 *   2. Active weapon slot: the target evaluates lcg_rand(3) FIRST (store +3),
 *      then ammo +1 = 0x10, then keyboard_state_machine() for the type byte +0.
 *      Our C had keyboard first; reversed to match the call/store order.
 *   3. Weapon-slot fields are addressed by fully-inline byte arithmetic
 *      (g_player_recs + p*0x417 + 0x11d + j*0x28 + field) at every store rather
 *      than through a cached `slot` pointer, so the compiler re-derives the
 *      stride per store from the memory-homed j/p counters and folds the
 *      region-base+field into the displacement -- matching the target's shape.
 *
 * Records use odd byte strides (0x417, 0x1eb, 0x1f5) so fields are addressed by
 * byte-pointer arithmetic + cast, written inline at each store (not through a
 * cached base pointer) -- that re-derives the stride per store from the
 * memory-homed counter, matching the target's shape.
 *
 * Remaining gap character (compiled -4s, against the true 1675-byte target):
 * first diff at 0x6 -- the frame `sub esp,0x18` where ours is `sub esp,0x10`,
 * because the target memory-homes two more counter spill slots, so p/j land at
 * different [esp] displacements and every counter reload differs by one
 * displacement byte. The target also addresses record fields by folding the
 * absolute array base into a disp32 (e.g. mov [ebx+0xe5b9]) where ours keeps
 * the base in a register and uses a small field displacement (mov [edx+0x11d]);
 * both are relocated disp32 forms of the same address. Plus scattered
 * immediate-vs-register choices for the 0x960/0x10/0x1e constants. All
 * source-invisible allocator/encoder choices.
 *
 * GHIDRA MIS-DISASSEMBLY: two spots where Watcom emitted 6-byte
 * `lea reg,[reg+0]` alignment NOPs between an `imul` and the next store (raw
 * 8d 80 00 00 00 00 / 8d 92 00 00 00 00) are mis-decoded as PUSHAD / OR /
 * JMP [ESI-0x77] (@0x2140e) and ADD AL,imm (@0x215d2). Recovered from raw
 * bytes: both are the ordinary `g_5788[..]=0x960` / `g_7c07[..]=0` loop heads.
 * There is no data-in-code.
 *
 * ---- Tables initialised ----
 *  8 player command records (0x105d4, stride 0xe)
 *  8 player equip/research templates (0xe49c, stride 0x417): money, roster,
 *    18 weapon/mod slots each (40-byte slots at +0x11d)
 *  50 syndicate records (0x539c, stride 0xa): random starting money
 *  18 research records (0x5788, stride 0x1eb) + a 10x24 word sub-grid at +2
 *  20 mod/equip records (0x7bf4, stride 0x1f5) + a 10x24 word sub-grid at +0x13
 *  a 0x1e9-byte flat state block (0x5594), memset to 0
 *
 * g_unlimited_funds = "unlimited funds" flag (network / debug): money 100,000,000 vs
 * 30,000, and constant 0x960 vs the g_research_src/g_mod_src source tables.
 * g_keep_synd_colours = "keep syndicate colours" flag. g_cur_player = current player, reset 0.
 */
extern unsigned char g_command_recs[];   /* 8 command records, stride 0xe            */
extern unsigned char g_player_recs[];    /* 8 player templates, stride 0x417         */
extern unsigned char g_syndicate_recs[];    /* 50 syndicate records, stride 0xa         */
extern unsigned char g_5788[];    /* 18 research records, stride 0x1eb        */
extern unsigned char g_7bf4[];    /* 20 mod records, stride 0x1f5             */
extern unsigned char g_5594[];    /* flat state block                         */
extern unsigned char g_roster_index[];   /* 4-entry roster index table               */
extern short g_research_src[];            /* research source table (word)             */
extern short g_mod_src[];            /* mod source table (word)                  */
extern unsigned char g_mod_byte_src[];    /* mod byte source table                    */
extern short g_rng_seed;               /* seed word copied into each command rec   */
extern short g_a73e;              /* default value for slot-0 of each mod list */
extern short g_cur_player;             /* current player (reset to 0)              */
extern unsigned char g_unlimited_funds;     /* unlimited-funds flag                     */
extern unsigned char g_keep_synd_colours;     /* keep-syndicate-colours flag              */

extern int lcg_rand(int n);            /* random in [0,n)                 */
extern unsigned char keyboard_state_machine(void);   /* random weapon/type byte         */
extern void fill_bytes(unsigned char *dst, int val, int len); /* memset      */

void new_campaign_reset(void)
{
    unsigned short p, i, j, k, di, bx;

    g_cur_player = 0;

    /* ---- per-player state: 8 players ---- */
    for (p = 0; p < 8; p++) {

        /* command record (0x105d4, stride 0xe) */
        g_command_recs[p * 0xe + 0xc] = (unsigned char)p;
        *(short *)(g_command_recs + p * 0xe + 0xa) = 0;
        *(short *)(g_command_recs + p * 0xe + 0) = 0;
        *(short *)(g_command_recs + p * 0xe + 2) = 0;
        *(short *)(g_command_recs + p * 0xe + 4) = 0;
        g_command_recs[p * 0xe + 0xd] = 0;
        *(short *)(g_command_recs + p * 0xe + 6) = g_rng_seed;

        /* equip/research template (0xe49c, stride 0x417) */
        if (p == 0) {
            g_player_recs[p * 0x417 + 0x11] = 0;   /* g_e4ad */
            g_player_recs[p * 0x417 + 0xe]  = 1;   /* g_player_flags: player 0 human/active */
        } else {
            g_player_recs[p * 0x417 + 0x11] = 0;
            g_player_recs[p * 0x417 + 0xe]  = 2;   /* other players AI syndicates */
        }

        for (i = 0; i < 0x32; i++) {
            g_player_recs[p * 0x417 + 0xb9 + i] = 0;   /* g_e555: roster A */
            g_player_recs[p * 0x417 + 0xeb + i] = 0;   /* g_e587: roster B */
        }

        g_player_recs[p * 0x417 + 0x10] = (unsigned char)p;   /* g_e4ac */
        g_player_recs[p * 0x417 + 0xf]  = (unsigned char)p;   /* g_player_owner: owner id */
        *(int *)(g_player_recs + p * 0x417 + 4)    = 0;        /* g_player_budget */
        *(short *)(g_player_recs + p * 0x417 + 8)  = 1;        /* g_e4a4 */
        *(short *)(g_player_recs + p * 0x417 + 0xa) = 0x55;    /* g_e4a6 */
        if (g_unlimited_funds)
            *(int *)(g_player_recs + p * 0x417 + 0) = 0x5f5e100;  /* 100,000,000 */
        else
            *(int *)(g_player_recs + p * 0x417 + 0) = 0x7530;     /* 30,000 */
        g_player_recs[p * 0x417 + 0xb6] = 0;                   /* g_agent_tmpl */
        g_player_recs[p * 0x417 + 0x11] = 0;                   /* g_e4ad */
        g_player_recs[p * 0x417 + 0x23] = 0;                   /* g_e4bf */
        *(short *)(g_player_recs + p * 0x417 + 0xc) = 0;       /* g_e4a8 */
        g_player_recs[p * 0x417 + 0xb5] = (unsigned char)(p * 8);  /* g_agent_slots base slot */

        /* 18 weapon/mod slots per player, 40-byte records at +0x11d.
         * Address re-derived inline per store (p*0x417 + j*0x28), matching the
         * target's memory-homed-counter + folded-displacement codegen. */
        for (j = 0; j < 0x12; j++) {
            if (j < 8) {                                    /* active slot */
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 3) = (short)(lcg_rand(3) & 1);
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 1) = 0x10;   /* ammo */
                g_player_recs[p * 0x417 + 0x11d + j * 0x28] = keyboard_state_machine();  /* type */
            } else {                                        /* empty slot */
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 1) = -1;
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 3) = 0;
                g_player_recs[p * 0x417 + 0x11d + j * 0x28] = 0xff;
            }
            if (j < 4) {
                g_player_recs[p * 0x417 + 0x11d + j * 0x28 + 7] = (unsigned char)(j + 1);   /* g_squad_slot */
                g_roster_index[j] = (unsigned char)j;
            } else {
                g_player_recs[p * 0x417 + 0x11d + j * 0x28 + 7] = 0;
            }
            for (k = 0; k < 8; k++) {
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 8 + k * 4) = 0;    /* g_equip_qty */
                *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 0xa + k * 4) = 0;  /* g_equip_kind */
            }
            *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 0xa) = 2;              /* g_equip_kind[0] */
            *(short *)(g_player_recs + p * 0x417 + 0x11d + j * 0x28 + 8) = g_a73e;           /* g_equip_qty[0] */
        }
    }

    /* ---- 50 syndicate records (0x539c, stride 0xa): starting money ---- */
    for (i = 0; i < 0x32; i++) {
        if (g_keep_synd_colours && i != 0)
            g_syndicate_recs[i * 0xa + 2] = 0;                         /* g_539e */
        else
            g_syndicate_recs[i * 0xa + 2] = (unsigned char)(lcg_rand(7) + 1);
        *(short *)(g_syndicate_recs + i * 0xa + 0) = 0;                /* g_syndicate_recs */
        *(int *)(g_syndicate_recs + i * 0xa + 6) =
            ((lcg_rand(0x14) & 0xffff) + 0x28) * 1000000; /* g_syndicate_money money */
        g_syndicate_recs[i * 0xa + 3] = 0x1e;                          /* g_539f */
        g_syndicate_recs[i * 0xa + 4] = 0x1e;                          /* g_53a0 */
    }

    /* ---- 18 research records (0x5788, stride 0x1eb) ---- */
    for (i = 0; i < 0x12; i++) {
        if (g_unlimited_funds) {
            *(short *)(g_5788 + i * 0x1eb + 0) = 0x960;
            for (di = 0; di < 0xa; di++)
                for (bx = 0; bx < 0x18; bx++)
                    *(short *)(g_5788 + i * 0x1eb + 2 + di * 0x30 + bx * 2) = 0;
        } else {
            *(short *)(g_5788 + i * 0x1eb + 0) = g_research_src[i];
            for (di = 0; di < 0xa; di++)
                for (bx = 0; bx < 0x18; bx++)
                    *(short *)(g_5788 + i * 0x1eb + 2 + di * 0x30 + bx * 2) = 0;
        }
    }

    /* ---- 20 mod/equip records (0x7bf4, stride 0x1f5) ---- */
    for (di = 0; di < 0x14; di++) {
        if (g_unlimited_funds) {
            if (g_7bf4[di * 0x1f5] == 0xfe)
                continue;                              /* already unavailable */
            *(short *)(g_7bf4 + di * 0x1f5 + 0x11) = 0x960;   /* g_7c05 */
            for (k = 0; k < 0xa; k++)
                for (bx = 0; bx < 0x18; bx++)
                    *(short *)(g_7bf4 + di * 0x1f5 + 0x13 + k * 0x30 + bx * 2) = 0;
        } else {
            *(short *)(g_7bf4 + di * 0x1f5 + 0x11) = g_mod_src[di];
            for (k = 0; k < 0xa; k++)
                for (bx = 0; bx < 0x18; bx++)
                    *(short *)(g_7bf4 + di * 0x1f5 + 0x13 + k * 0x30 + bx * 2) = 0;
        }
        g_7bf4[di * 0x1f5 + 0x1f4] = g_mod_byte_src[di];             /* g_7de8 */
    }

    /* ---- flat state block ---- */
    fill_bytes(g_5594, 0, 0x1e9);                /* memset(g_5594, 0, 0x1e9) */
    *(int *)(g_5594 + 3) = -1;                     /* g_5597 */
    g_5594[0] = 2;
}
