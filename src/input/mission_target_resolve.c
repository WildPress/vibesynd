/* frameless @ 0x2ad58 -- mission targeting-cursor / action RESOLUTION dispatcher.
   TRUE SIZE 3694 bytes. Produces the "cursor action descriptor" *p that the caller
   turns into a click order (move / attack / select-ped / pick-up / map-scroll ...).

   STATUS (prior pass): reconstructed the whole missing second half (the switch(g_cursor_mode)
   dispatch, ~1960 bytes, 54% of the fn). Logic is COMPLETE: all 25 action-code stores
   (p[0xd] = 0x2..0x1a, incl. the three 0x8 forms and two 0x16) are emitted -- verified
   count/value/order-identical to the target, so NO dispatch case is missing.

   STATUS (this pass): the -281B length shortfall was NOT missing logic -- it was
   infidelity in how we materialise the agent index. The target reads g_cur_player
   (0x10b16) exactly 12 times and emits exactly 12 SHL5;ADD;LEA*4;SUB;LEA*8;SUB *0x417
   chains: it NEVER caches g_cur_player, recomputing the chain fresh at every use. Our
   build hoisted/CSE'd it. Modelling the original by declaring `g_cur_player` volatile
   reproduces the 12 reloads and closes the gap: length delta -281 -> -33, ours 3413 ->
   3661 of 3694B; instruction-aligned byte match 2176 -> 2201, aligned-insn coverage
   60.8% -> 61.2%. What remains is codegen-tie residue, not missing behaviour -- FLOOR.

   PROLOGUE: push ebx;esi;edi; sub esp,4. The 4-byte [esp] slot is the return value
   `r` (modelled here as `volatile int r`, which reproduces the observed memory-homed
   spill: every `return` = `r = v; mov [esp],v; mov eax,[esp]; add esp,4; pops; ret`).
   Param `ushort *p` at [esp+0x14]. Output descriptor layout:
       p[0]=word (target x / ped-id / region-id), p[1]=word y/scale, p[2]=word z,
       byte p[0xd] = ACTION CODE.

   RECORD FAMILIES:
   (a) 0x417-stride AGENT TEMPLATE records, idx=g_cur_player. The SHL5;ADD;LEA*4;SUB;
       LEA*8;SUB chain materialises idx*0x417 to index byte tables g_agent_slots
       (AGENT_FIRST = agent's first ped pool-id) and g_agent_tmpl (AGENT_SEL = signed
       selection offset). Appears ~12x.
   (b) pool-A ENTITY records (0x5c stride): rec = g_pool_a + id*0x5c (base 0x8110 =
       g_entity_pool+2). Fields: +0x4/+0x6/+0x8 coords, +0xb/+0x1d flags, +0x19 type,
       +0x1a facing, +0x44 link-id, +0x46 byte. id<->ptr via the 0x810e (=g_pool_a-2) base.
   Direction tables g_dir_dx/g_dir_dy[256]. Screen/scroll g_mouse_x/g_mouse_y/g_map_zoom.
   g_5114[] = 4 mission-map HUD regions (18-byte stride, .x/.y at +0/+2).

   STRUCTURE (top to bottom):
     * early guards: g_e285/radar -> 0x2; g_e2a4 -> 0x10; g_e28x -> set g_sel_agent;
       g_e297&g_e296&g_e2a3 -> 0x17.
     * fast path (already have a target ped selected in g_target_id): -> 0x1a, or set
       g_sel_agent to the ped index if g_cursor_mode==1.
     * scan the agent's 4-ped block for a shootable ped -> confirmed selection -> 0x6.
     * pick a fresh target under the cursor (type/range/link checks) -> 0xf.
     * adjacency / line-of-sight pass over the block (g_cursor_mode==2) or the selected ped
       -> 0x16.
     * clamp cursor to world/scroll, set the reticle window (g_reticle_z/1e/20), g_10b39.
     * draw_targeting_reticle(rec).
     * FINAL DISPATCH  switch (g_cursor_mode)  (orig uses a 5-entry jump table @ cs:0x1d5fc):
         case 0            -> return.
         case 1 / case 2   -> movement/aim commit CHAIN (0x2b44c..): a sequence of
                              guarded emitters, each returns or falls to the next:
                                g_10b41&g_10b40        -> 0x11
                                g_10b39&g_target_pending        -> 0x3
                                g_target_pending&g_10b22>=0x80  -> commit pending target:
                                    g_10b12 -> 0x19 or 0x9 ; g_10b1a -> 0x5 ;
                                    g_target_id -> 0x18/0x4 ; else -> 0x3
                                g_10b39&g_10b3e        -> clamp, no p action
                                g_10b40&mouse&link     -> pick nearest -> 0x8 (x3 forms)
                                g_e118: g_e114 -> 0xa ; g_sel_cursor: 0xb / 0x7
                              if none fire, FALLS THROUGH into:
         case 3 / 4 / dflt -> map-region hit-test (0x2b91e): if g_10b41 && cursor in
                              HUD, loop the 4 g_5114 regions; three vertical bands emit
                              0xc / 0xe / 0xd with p[1] = clamp((mouse_x-(x0+4))*256/0x37).

   REMAINING (codegen-tie FLOOR, delta now -33B, all register/shape not logic):
   1. Register allocation: Watcom gives our build a 4th callee-saved reg (push ebp +
      pop ebp at every return); the original manages the whole body in ebx/esi/edi +
      the [esp] slot. This one decision costs ~35 bytes (1 push, ~24 pops, ebp-as-zero
      at returns), shifts the param offset, and desyncs the instruction-aligned diff at
      instr 0 (regdiff cannot see past it). The original holds `rec` across the
      draw_targeting_reticle call and the whole switch in a general temp; our C needs a
      callee-saved reg for it. Not steerable from source without a register-pressure
      change we could not find (the documented hard wall).
   1b.Loop shape: the target's block-scan loops recompute the AGENT_FIRST(+4) bound each
      iteration (jmp-to-bottom, single test) with the *0x5c stride hoisted in a reg;
      ours rotates the loop (test duplicated top+bottom) with the bound in a reg. Same
      semantics, ~15-50B per loop of shape difference, below the flags' control.
   2. Widen-form / register-role ties across the two walled record families: byte loads
      `mov al,[..+0xe551]; and eax,0xff` vs `movsx`, and which GPR holds rec/counter/temp.
      These are the documented Watcom-9.5 wall (same class that parks 0x2d7a8).
   3. The switch built as a compare-chain (3 distinct case-groups) instead of the
      original's jump table -- below Watcom's table-emission threshold. Restoring the
      table needs 5 physically-distinct case bodies, which the semantics don't have.

   The body below is a faithful structural decode (real C, semantics-exact). */

extern unsigned char  g_e285, g_sel_agent1_req, g_sel_agent2_req, g_sel_agent3_req, g_sel_agent4_req;
extern unsigned char  g_e296, g_e297, g_e2a3, g_e2a4;
extern unsigned char  g_10b3e, g_target_pending, g_10b39, g_10b40, g_radar_detail;
extern unsigned short g_e112, g_e114, g_sel_cursor, g_e118, g_e11a, g_e11c;
extern unsigned short g_cursor_mode, g_e122, g_sel_agent;
extern short          g_10b10, g_10b12, g_target_id, g_10b1a;
extern volatile short g_cur_player;   /* orig NEVER caches this: 12 uses -> 12 reloads + 12 *0x417 chains */
extern unsigned short g_reticle_z, g_reticle_y, g_reticle_x, g_cursor_x;
extern unsigned short g_mouse_x, g_mouse_y, g_map_zoom;
extern unsigned char  g_10b41;
extern unsigned short g_10b22, g_10b24;
extern unsigned char  g_agent_slots[], g_agent_tmpl[];   /* 0x417-stride agent template byte tables */
extern short          g_dir_dx[], g_dir_dy[];   /* direction tables (s16[256]) */

/* mission-map region table (0x5114, 18-byte stride): 4 HUD/map slots. */
struct hud_region { unsigned short x, y; unsigned char pad[14]; };
extern struct hud_region g_5114[];

/* pool-A entity record k -> byte ptr (0x8110 + k*0x5c). base = g_entity_pool+2. */
extern unsigned char g_pool_a[];

extern int  clamp_point_box(short x, short y);
extern void copy_5fields_8recs(void);
extern int  interp_scale_a(unsigned char *rec, int a);
extern int  interp_scale_b(unsigned char *rec, int a);
extern int  FUN_00036be8(unsigned char *rec, unsigned short *node);
extern int  pool_table_lookup(unsigned char *rec);   /* @0x377e8; caller also calls this as "ped_check_a" */
extern void draw_targeting_reticle(unsigned char *rec);
extern unsigned char *pool_resolve(unsigned char *rec, int a);

#define AGENT_FIRST(idx)  ((unsigned int)g_agent_slots[(int)(idx) * 0x417])
#define AGENT_SEL(idx)    ((int)(signed char)g_agent_tmpl[(int)(idx) * 0x417])
#define PREC(k)           (g_pool_a + (unsigned int)(k) * 0x5c)   /* pool-A record k */

int mission_target_resolve(unsigned short *p)
{
    int idx, first, sel;
    unsigned char *rec, *end, *q;
    volatile int r;           /* memory-homed return slot (sub esp,4; ret via [esp]) */

    if (g_e285 && g_radar_detail) { *((unsigned char *)p + 0xd) = 0x2;  r = 0; return r; }
    if (g_e2a4 && !g_radar_detail){ *((unsigned char *)p + 0xd) = 0x10; r = 0; return r; }

    if (g_sel_agent1_req) g_sel_agent = 1;
    if (g_sel_agent2_req) g_sel_agent = 2;
    if (g_sel_agent3_req) g_sel_agent = 3;
    if (g_sel_agent4_req) g_sel_agent = 4;

    if (g_e297 && g_e296 && g_e2a3) { *((unsigned char *)p + 0xd) = 0x17; r = 0; return r; }

    /* ---- "already have a target ped selected" fast path ---- */
    if (g_target_id != 0 && g_target_pending != 0 && !(g_10b1a != 0 && g_target_pending != 0)) {
        idx   = g_cur_player;
        first = AGENT_FIRST(idx);
        rec   = PREC(g_target_id);                 /* node = ped id in g_target_id */
        end   = PREC(first);                   /* agent's ped-block start  */
        if (rec >= end && rec < PREC(first + 4)) {
            if (g_cursor_mode == 1) {
                /* which ped index within the agent's 4-ped block */
                g_sel_agent = (unsigned short)((rec - end) / 0x5c + 1);
                r = 0; return r;
            }
            p[0] = g_reticle_x; p[1] = g_reticle_y; p[2] = g_reticle_z;
            *((unsigned char *)p + 0xd) = 0x1a;
            r = 0; return r;
        }
    }

    /* ---- main resolution (label 0x2af1c) ---- */
    idx = g_cur_player;
    rec = PREC(AGENT_FIRST(idx) + AGENT_SEL(idx));   /* selected ped record */
    if ((*(rec + 0xb) & 1) == 0 && (*(rec + 0x1d) & 4) != 0)
        goto after_scan;                             /* -> 0x2b01b */

    /* scan the agent's ped block for a shootable target (0x2af88..0x2b019) */
    g_sel_agent = 0;
    {
        unsigned char *e = PREC(AGENT_FIRST(g_cur_player) + 4);
        unsigned short si = 0;
        q = PREC(AGENT_FIRST(g_cur_player));
        while (q < e) {
            if ((*(q + 0xb) & 1) == 0 && (*(q + 0x1d) & 4) != 0) {
                g_e122 = g_sel_agent;
                g_sel_agent = ++si;
                g_e112 = 1;
                goto after_scan;
            }
            q += 0x5c; ++si;
        }
    }

after_scan:                                          /* 0x2b01b */
    {
        int b = AGENT_FIRST(g_cur_player);
        rec = PREC(b + (g_sel_agent - 1));
        if (g_e112 != 0 && g_sel_agent != 0 && (*(rec + 0x1d) & 4) != 0) {
            /* confirmed selection -> build move/attack order (0x2b06c) */
            if (g_sel_cursor != 0) g_cursor_mode = 1;
            copy_5fields_8recs();
            *((unsigned char *)p + 0xd) = 0x6;
            p[0] = g_sel_agent - 1;
            if (g_e114 != 0) {
                int z = *(short *)(rec + 0x8);
                clamp_point_box((*(short *)(rec + 0x4) - 0xa00 - z) >> 7,
                             (*(short *)(rec + 0x6) - 0x200 - z) >> 7);
            }
            g_target_pending = 0; g_10b3e = g_target_pending;
            g_sel_cursor = g_10b3e; g_e114 = g_10b3e;
            r = 0; return r;
        }
    }

    /* ---- pick a fresh target under the cursor (0x2b11c) ---- */
    rec = PREC(AGENT_FIRST(g_cur_player) + AGENT_SEL(g_cur_player));
    if (*(rec + 0x19) == 0
        && interp_scale_a(rec, 0xa) > 0
        && interp_scale_b(rec, 0x64) > 0
        && *(short *)(rec + 0x44) != 0
        && *(rec + 0x46) == 0) {
        int f = *(rec + 0x1a);
        *((unsigned char *)p + 0xd) = 0xf;
        p[0] = (unsigned short)(((g_dir_dx[f] << 0xa) >> 8) + *(short *)(rec + 0x4));
        p[1] = (unsigned short)(((g_dir_dy[f] << 0xa) >> 8) + *(short *)(rec + 0x6));
        p[2] = g_reticle_z;
    }

    /* ---- adjacency / line-of-sight passes over the ped block (0x2b1d7 / 0x2b28f) ---- */
    if (g_cursor_mode == 2) {
        unsigned char *e = PREC(AGENT_FIRST(g_cur_player) + 4);
        unsigned short di = 0;
        q = PREC(AGENT_FIRST(g_cur_player));
        while (q < e) {
            if ((*(q + 0x1d) & 4) != 0) {
                unsigned short lk = *(unsigned short *)(q + 0x44);
                if (di != lk) {
                    unsigned char *o = g_pool_a - 2 + lk;   /* 0x810e + lk */
                    if ((short)di > *(short *)(o + 0x14) && *(q + 0x19) != 0xa) {
                        *((unsigned char *)p + 0xd) = 0x16;
                        p[0] = *(o + 0x19);
                    }
                }
            }
            q += 0x5c;
        }
    } else {
        rec = PREC(AGENT_FIRST(g_cur_player) + AGENT_SEL(g_cur_player));
        {
            unsigned short lk = *(unsigned short *)(rec + 0x44);
            if (lk != 0) {
                unsigned char *o = g_pool_a - 2 + lk;
                if (*(short *)(o + 0x14) < 0 && *(rec + 0x19) != 0xa) {
                    *((unsigned char *)p + 0xd) = 0x16;
                    p[0] = *(o + 0x19);
                }
            }
        }
    }

    /* ---- clamp cursor to world/scroll and set the reticle window (0x2b2f8) ---- */
    rec = PREC(AGENT_FIRST(g_cur_player) + AGENT_SEL(g_cur_player));
    if (g_mouse_x >= 0x80 && g_mouse_y >= 0x110) {
        int t;
        t = ((int)(0x100 / (int)g_map_zoom)) * ((int)g_mouse_x - 0x40) + *(short *)(rec + 0x4);
        g_reticle_x = (unsigned short)t;
        if ((unsigned short)t > 0x7f00) g_reticle_x = 0x7f00;
        else if ((unsigned short)t < 0x100) g_reticle_x = 0x100;

        t = ((int)(0x100 / (int)g_map_zoom)) * ((int)g_mouse_y - 0x150) + *(short *)(rec + 0x6);
        g_reticle_y = (unsigned short)t;
        if ((unsigned short)t > 0x5f00) g_reticle_y = 0x5f00;
        else if ((unsigned short)t < 0x100) g_reticle_y = 0x100;

        g_reticle_z = *(unsigned short *)(rec + 0x8) + 0x80;
        g_10b39 = 1;
    } else {
        g_10b39 = 0;
    }

    draw_targeting_reticle(rec);          /* 0x2b41d push esi; call cursor draw */

    /* ---- final mode dispatch: switch (g_cursor_mode) (jump table @ cs:0x1d5fc) ---- */
    switch (g_cursor_mode) {
    case 0:
        r = 0; return r;                                        /* 0x2b43d */

    case 1:
    case 2:
        /* ==== movement/aim commit chain (0x2b44c..0x2b91e) ==== each guarded
           test either emits an action into p and returns, or falls through to the
           next. If none fire, control drops into the 0xbc6 map-region block. */

        if (g_10b41 && g_10b40) {                        /* 0x2b44c -> action 0x11 */
            *((unsigned char *)p + 0xd) = 0x11;
            p[0] = g_reticle_x; p[1] = g_reticle_y; p[2] = g_reticle_z;
            g_target_pending = 0; g_10b3e = 0;
            g_e114 = 0; g_sel_cursor = 0;
            r = 0; return r;
        }
        if (g_10b39 && g_target_pending) {                         /* 0x2b4aa -> action 0x3 */
            *((unsigned char *)p + 0xd) = 0x3;
            p[0] = g_reticle_x; p[1] = g_reticle_y; p[2] = g_reticle_z;
            g_target_pending = 0; g_sel_cursor = 0;
            r = 0; return r;
        }
        if (g_target_pending && g_10b22 >= 0x80) {                 /* 0x2b4fd -- commit pending target */
            if (g_10b12 != 0) {                           /* 0x2b525 */
                if (FUN_00036be8(rec, (unsigned short *)(g_pool_a - 2 + g_10b12)) != 0)
                    *((unsigned char *)p + 0xd) = 0x19;
                else {
                    *((unsigned char *)p + 0xd) = 0x9;
                    p[0] = g_10b12;
                }
                g_target_pending = 0; g_sel_cursor = 0;
                r = 0; return r;
            }
            if (g_10b1a != 0) {                           /* 0x2b570 -> action 0x5 */
                *((unsigned char *)p + 0xd) = 0x5;
                g_sel_cursor = 0;
                p[0] = g_10b1a;
                g_target_pending = 0;
                r = 0; return r;
            }
            if (g_target_id != 0) {                           /* 0x2b5a1 -> action 0x18/0x4 */
                unsigned char *o = g_pool_a - 2 + g_target_id;
                if (o == rec) *((unsigned char *)p + 0xd) = 0x18;
                else          *((unsigned char *)p + 0xd) = 0x4;
                p[0] = g_10b10;
                g_target_pending = 0; g_sel_cursor = 0;
                r = 0; return r;
            }
            *((unsigned char *)p + 0xd) = 0x3;            /* 0x2b5f2 -> action 0x3 */
            p[0] = g_reticle_x; p[1] = g_reticle_y; p[2] = g_reticle_z;
            g_target_pending = 0; g_sel_cursor = 0;
            r = 0; return r;
        }
        if (g_10b39 && g_10b3e) {                         /* 0x2b62f -- clamp reticle */
            clamp_point_box((g_reticle_x - 0xa00 - (int)g_reticle_z) >> 7,
                            (g_reticle_y - 0x200 - (int)g_reticle_z) >> 7);
            g_10b3e = 0; g_e114 = 0;
            r = 0; return r;
        }
        if (g_10b40 && g_mouse_x >= 0x80 && *(short *)(rec + 0x44) != 0) {   /* 0x2b69f */
            if (pool_table_lookup(rec) != 0 || g_10b3e != 0) {
                if (g_10b3e != 0) g_10b3e = 0;
                if (g_target_id != 0 && (g_pool_a - 2 + g_target_id) != rec) {   /* 0x2b6fb -> action 0x8 */
                    unsigned char *o = g_pool_a - 2 + g_target_id;
                    *((unsigned char *)p + 0xd) = 0x8;
                    p[0] = *(unsigned short *)(o + 4);
                    p[1] = *(unsigned short *)(o + 6);
                    g_e114 = 0;
                    p[2] = *(unsigned short *)(o + 8) + 0x80;
                    r = 0; return r;
                }
                if (g_10b12 != 0) {                       /* 0x2b74b -> action 0x8 */
                    unsigned char *o = g_pool_a - 2 + g_10b12;
                    *((unsigned char *)p + 0xd) = 0x8;
                    p[0] = *(unsigned short *)(o + 4);
                    p[1] = *(unsigned short *)(o + 6);
                    g_e114 = 0;
                    p[2] = *(unsigned short *)(o + 8) + 0x80;
                    r = 0; return r;
                }
                *((unsigned char *)p + 0xd) = 0x8;        /* 0x2b797 -> action 0x8 */
                p[0] = g_reticle_x;
                g_e114 = g_10b12;
                p[1] = g_reticle_y;
                p[2] = g_reticle_z;
                r = 0; return r;
            }
        }
        if (g_e118 != 0) {                                /* 0x2b7cc */
            if (g_e114 != 0) {                            /* 0x2b7da -> action 0xa */
                unsigned char *r2 = pool_resolve(
                        PREC(AGENT_FIRST(g_cur_player) + AGENT_SEL(g_cur_player)), g_e11c);
                *((unsigned char *)p + 0xd) = 0xa;
                g_e114 = 0;
                p[0] = (unsigned short)(r2 - (g_pool_a - 2));
                g_10b3e = 0;
                r = 0; return r;
            }
            if (g_sel_cursor != 0) {                      /* 0x2b860 */
                if (g_e11c == g_e11a) {                   /* 0x2b880 -> action 0xb */
                    g_e11c = 0;
                    *((unsigned char *)p + 0xd) = 0xb;
                    g_target_pending = 0;
                    g_sel_cursor = 0;
                    r = 0; return r;
                }
                {                                         /* 0x2b8a7 -> action 0x7 */
                    unsigned char *r2 = pool_resolve(
                            PREC(AGENT_FIRST(g_cur_player) + AGENT_SEL(g_cur_player)), g_e11c);
                    *((unsigned char *)p + 0xd) = 0x7;
                    p[0] = (unsigned short)(r2 - (g_pool_a - 2));
                    g_target_pending = 0;
                    g_sel_cursor = 0;
                    r = 0; return r;
                }
            }
        }
        /* fall through into the 0xbc6 map-region hit-test */

    case 3:
    case 4:
    default:
        /* ==== 0x2b91e -- hit-test cursor (g_10b22/g_10b24) against the 4 map
           regions in g_5114; three vertical bands emit actions 0xc / 0xe / 0xd. ==== */
        if (g_10b41 && g_10b22 < 0x80) {
            short i;
            for (i = 0; i < 4; i++) {
                int x0 = g_5114[i].x;
                int y0 = g_5114[i].y;
                if (g_10b22 >= x0 + 4 && g_10b22 <= x0 + 0x3b
                    && g_10b24 >= y0 + 0x2e && g_10b24 <= y0 + 0x3c) {   /* action 0xc */
                    short v;
                    *((unsigned char *)p + 0xd) = 0xc;
                    p[0] = i;
                    v = (short)(((g_mouse_x - (x0 + 4)) << 8) / 0x37);
                    if (v < 0) p[1] = 0; else if (v > 0xff) p[1] = 0xff; else p[1] = v;
                    g_target_pending = 0;
                    r = 0; return r;
                }
                if (g_10b22 >= x0 + 4 && g_10b22 <= x0 + 0x3b
                    && g_10b24 >= y0 + 0x3c && g_10b24 <= y0 + 0x4a) {   /* action 0xe */
                    short v;
                    *((unsigned char *)p + 0xd) = 0xe;
                    p[0] = i;
                    v = (short)(((g_mouse_x - (x0 + 4)) << 8) / 0x37);
                    if (v < 0) p[1] = 0; else if (v > 0xff) p[1] = 0xff; else p[1] = v;
                    g_target_pending = 0;
                    r = 0; return r;
                }
                if (g_10b22 >= x0 + 4 && g_10b22 <= x0 + 0x3b
                    && g_10b24 >= y0 + 0x4a && g_10b24 <= y0 + 0x58) {   /* action 0xd */
                    short v;
                    *((unsigned char *)p + 0xd) = 0xd;
                    p[0] = i;
                    v = (short)(((g_mouse_x - (x0 + 4)) << 8) / 0x37);
                    if (v < 0) p[1] = 0; else if (v > 0xff) p[1] = 0xff; else p[1] = v;
                    g_target_pending = 0;
                    r = 0; return r;
                }
            }
        }
        break;
    }
    return r;                 /* 0x2bbbc -- falloff loads the slot unchanged */
}
