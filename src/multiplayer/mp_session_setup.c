/* PARKED NEAR-MISS (ours 1474B vs target 1482B; structure instruction-for-
 * instruction exact -- every one of the 11 calls, both inline far strcpy
 * blocks, the near strcpy+strlen name build, and all five loops match in
 * FORM).  Two residual wall classes, both inherited from this NetBIOS/DPMI
 * subsystem (same family that walls siblings 0x28118/0x28228/0x28368):
 *
 * 1. ENTRY DEAD-INIT (root, 9B).  Target keeps `mov edx,1; mov [esp+0x4c],edx`
 *    -- a dword init of the 16-bit loop-counter slot whose upper half is
 *    provably dead (every read is word/movsx, every store `mov word[..],si`).
 *    Our identical 9.5b compiler eliminates it (i=1 is dead: overwritten by the
 *    clear loop's i=0).  Tried: `short i=1`, `i=1` statement, `*(int*)&i=1`
 *    punned dword write -- all dead-store-eliminated.  There is no live 32-bit
 *    read of i to anchor the init (the read loop never touches [0x4c]); the
 *    original compilation retained a truly-dead store we cannot force from
 *    clean source.  Same class as the cont.21 "dead test reg,reg" family but
 *    with no recoverable source form.
 * 2. REGISTER-ROLE cascade (downstream, pervasive 1B flips).  With the init
 *    spliced in, positional bytes still diverge throughout: g_name_buf base lives
 *    in EBX (target) vs EBP (ours); assorted push esi/edi order; and the
 *    zero-register reuse in byte compares (`cmp byte gs:[eax+2],0` /
 *    `cmp byte[0xe285],0` in target vs `cmp dl,gs:2[eax]` / `cmp bl,[g_e285]`
 *    in ours -- Watcom value-numbers a 0 into a callee-saved reg and reuses it
 *    as the compare operand).  Named-base-pointer and volatile(g_e285) levers
 *    did not move either; these are the class-1/2 encoding/accumulator ties.
 *
 * LEVERS THAT LANDED (took ours 1332 -> 1474): `#pragma intrinsic(strcpy,
 * strlen)` REQUIRES the prototypes present or Watcom emits calls (declaring
 * both inlined the 2-byte-unroll strcpy + repne-scasb strlen byte-exact);
 * `#pragma intrinsic(_fstrcpy)` (declared) inlines the far strcpy blocks;
 * `short i` (not int) for the counter is the single biggest fix -- it word-
 * homes the counter AND defeats the EBP+=6 strength-reduction (movsx breaks
 * the induction chain, forcing the target's recompute-6*i-each-iteration);
 * `short count` for the 16-bit poll `cmp di,si`.  Fuzzer/cpermute (decl-perm)
 * may close the register-role residue; the entry dead-init likely will not.
 * Recipe: -4s -oneatx -zp8 -s -zq
 *
 * mp_session_setup @ 0x27428 - multiplayer session setup (NetBIOS/DPMI family of
 * 0x27fc8/0x28118/0x28228/0x28368/0x28628). Prompts for player count (2..8),
 * registers our name (g_name_buf + digit) via 0x27fc8 (retrying the delete 0x28118
 * on a -13 name-conflict), broadcasts our name to the other connection records
 * (far strcpy of the +0x1a name field + byte +3, then 0x28228), then tries to
 * connect to each peer via 0x28368, copying its +0xa name + byte +2 and
 * counting successes; finally spin-waits until every record is ready and
 * returns the connected count.  g_conn[] is the far-ptr connection table at
 * 0x10644 (off @ +0, sel @ +4, stride 6).
 */
extern short g_num_players;                 /* number of players */
extern short g_cur_player;                 /* our index (-1 until registered) */
extern unsigned char g_input_echo;         /* input-echo flag */
extern unsigned char g_e285;          /* abort/ESC flag */
extern unsigned char g_df30[];        /* per-player ready byte */
extern unsigned char g_player_status[];       /* per-player status (stride 14) */
extern char g_name_buf[];                 /* base session name */
extern char g_36b8[], g_36cc[], g_36ec[], g_370c[];
extern char g_372c[], g_3740[], g_375c[], g_3684[];
extern unsigned char __far *g_conn[]; /* connection records (far ptrs) @ 0x10644 */

extern void set_video_mode(int);
extern void FUN_0003b2b5(char *s);
extern void FUN_0003b326(char *buf);
extern long atol(char *s);
extern void printf(char *fmt, ...);
extern void sprintf(char *buf, char *fmt, ...);
extern int  submit_command(unsigned char __far *p, char *name);
extern int  xfer_buf_req_b1(unsigned char __far *p, char *name);
extern int  netbios_op91(unsigned char __far *p, char *name, unsigned short async, char rto, char sto);
extern int  netbios_op90(unsigned char __far *p, char *name, unsigned short async, char rto, char sto);
extern short FUN_00028628(unsigned char __far *p);

extern char *strcpy(char *dst, const char *src);
extern unsigned strlen(const char *s);
extern char __far *_fstrcpy(char __far *dst, char __far *src);
#pragma intrinsic(strcpy, strlen, _fstrcpy)

int mp_session_setup(void)
{
    char rbuf[44];
    char nbuf[20];
    unsigned char save;
    short i;
    int connected;
    short nt;

    save = g_input_echo;
    i = 1;
    g_input_echo = 1;
    set_video_mode(0x12);
readloop:
    FUN_0003b2b5(g_36b8);
    FUN_0003b326(rbuf);
    g_num_players = (short)atol(rbuf);
    if (g_num_players == 0) goto readloop;
    if (g_num_players > 8) goto readloop;
    if (g_num_players == 1) return -3;
    g_input_echo = save;
    printf(g_36cc);
    for (i = 0; i < g_num_players; i++)
        g_df30[i] = 0;
esccheck:
    if (g_e285 != 0) return -2;
    g_cur_player = -1;
    for (i = 0; i < g_num_players; i++) {
        int r;
        strcpy(nbuf, g_name_buf);
        nbuf[strlen(nbuf) + 1] = 0;
        nbuf[strlen(nbuf)] = (char)(i + 0x30);
        r = submit_command(g_conn[i], nbuf);
        if (r == 0) { g_cur_player = i; goto found; }
        if (r == -13) {
            xfer_buf_req_b1(g_conn[i], nbuf);
            i--;
        }
    }
found:
    if (g_cur_player == -1) goto esccheck;
    printf(g_36ec);
    for (i = 0; i < g_num_players; i++) {
        if (i != g_cur_player) {
            _fstrcpy((char __far *)(g_conn[i] + 0x1a), (char __far *)(g_conn[g_cur_player] + 0x1a));
            g_conn[i][3] = g_conn[g_cur_player][3];
            sprintf(nbuf, g_3684, g_name_buf, i);
            netbios_op91(g_conn[i], nbuf, 1, 0, 0);
        }
        g_df30[i] = 0;
        g_player_status[i * 14] = 0;
    }
    printf(g_370c);
    for (i = 0; i < g_num_players; i++) {
        int r;
        g_df30[i] = 0;
        if (i == g_cur_player) continue;
        if (g_conn[i][2] == 0) goto do_connect;
        if (g_conn[i][0x31] != 0xff) continue;
    do_connect:
        printf(g_372c, i, g_conn[i][2], g_conn[i][0x31]);
        sprintf(nbuf, g_3684, g_name_buf, i);
        r = netbios_op90(g_conn[g_cur_player], nbuf, 0, 0, 0);
        if (r != 0) continue;
        printf(g_3740, i);
        FUN_00028628(g_conn[i]);
        _fstrcpy((char __far *)(g_conn[i] + 0xa), (char __far *)(g_conn[g_cur_player] + 0xa));
        g_conn[i][2] = g_conn[g_cur_player][2];
        connected++;
        g_df30[i] = 1;
    }
    printf(g_375c);
    if (g_e285 != 0) return -2;
    nt = g_num_players;
poll:
    {
        short count = 0;
        for (i = 0; i < g_num_players; i++) {
            if (g_conn[i][2] == 0) goto check_my;
            if (g_conn[i][0x31] != 0xff) goto do_inc;
        check_my:
            if (i != g_cur_player) continue;
        do_inc:
            count++;
        }
        if (count != nt) {
            if (g_e285 != 0) return -2;
            goto poll;
        }
    }
    g_player_status[g_cur_player * 14] = 1;
    return connected;
}
