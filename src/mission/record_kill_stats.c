/* NEAR-MISS @ 0x2ed28 -- kill/hit stat bookkeeping. IMPROVED dist 66 -> 37 by
 * breaking the register-role rotation the old header called a wall.
 *
 * The fix was DECL/STATEMENT ORDER, not a byte-trick: computing `n` (the target
 * link record) BEFORE the agent-slot lookup `c` re-seeds Watcom's allocator so
 * ALL five role registers now match the target -- b=EBX (the one saved reg),
 * n=EDX, c=ECX, lo=EAX, hi=ECX(reused). The old layout (c-first) put
 * b=EDX/c=EBX, a whole-function rotation. Pairing n-first with `unsigned char c`
 * (byte type ranks c into ECX; `int`/`unsigned int` mis-rank it to EAX and
 * re-introduce the swap) lands lo=EAX/hi=ECX correctly too.
 *
 * REMAINING GAP (2 bytes, masked-diff): target widens the byte load as
 * `xor ecx,ecx; mov cl,[eax+&g_agent_slots]` (reuses w's dead ECX); ours emits
 * `mov al,[eax+..]; xor ecx,ecx; mov cl,al` (byte load reuses the index's dead
 * EAX, then copies AL->CL). Both dead registers are free at that point; which
 * one the byte load reuses is a Watcom register-selection tiebreak with no
 * source lever -- decl order, explicit index temp, (unsigned)/&0xff casts, and
 * all byte/word/dword c-types were tried; uchar+n-first is the floor at 37.
 *
   0x2ed28 semantics: b = pool record with target link at +0x16 (0 = none) and
 * cause flags at +0x1c. n = g_entity_pool + link. The player's own 4 agents
 * occupy pool-A records [g_pool_a + c*0x5c, g_pool_a + (c+4)*0x5c) where c =
 * first-agent index byte g_agent_slots[g_cur_player * 0x417]. If n is an own
 * agent, bump a counter by cause: &1 g_10af4, &2 g_10afa (shooter also own
 * agent) / g_10af8 (foreign), &0x10 g_10af7, &4 g_10af5, &8 g_10af6. Then if
 * n's flags have 1|2|0x10: either mark n[0x1d]|=2 (b is &4 and n isn't) or
 * n[0x1c]|=0x40. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_cur_player;
extern unsigned char g_agent_slots[];
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char g_10af4;
extern unsigned char g_10af5;
extern unsigned char g_10af6;
extern unsigned char g_10af7;
extern unsigned char g_10af8;
extern unsigned char g_10afa;

void record_kill_stats(unsigned char *b)
{
    int w;
    unsigned char c;
    unsigned char *lo;
    unsigned char *n;
    unsigned char *hi;

    if (*(unsigned short *)(b + 0x16) == 0)
        return;
    n = g_entity_pool + *(unsigned short *)(b + 0x16);
    w = g_cur_player;
    c = g_agent_slots[w * 0x417];
    lo = g_pool_a + c * 0x5c;
    if (n >= lo) {
        hi = g_pool_a + (c + 4) * 0x5c;
        if (n < hi) {
            if (b[0x1c] & 1)
                g_10af4++;
            else if (b[0x1c] & 2) {
                if (b >= lo && b < hi)
                    g_10afa++;
                else
                    g_10af8++;
            } else if (b[0x1c] & 0x10)
                g_10af7++;
            else if (b[0x1c] & 4)
                g_10af5++;
            else if (b[0x1c] & 8)
                g_10af6++;
        }
    }
    if ((n[0x1c] & 1) || (n[0x1c] & 2) || (n[0x1c] & 0x10)) {
        if ((b[0x1c] & 4) && !(n[0x1c] & 4)) {
            n[0x1d] |= 2;
            return;
        }
        n[0x1c] |= 0x40;
    }
}
