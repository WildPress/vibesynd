/* NEAR-MISS @ 0x2ed28 -- PARKED on a register-role rotation wall, improved
 * cont.21: `unsigned int c` (full-width temp lever) closed the widen-form gap
 * (and-form `mov bl;and ebx,0xff` -> target's xor-form `xor;mov cl`) and the
 * length now matches 230/230; ALL 36 remaining diff bytes are pure reg
 * encodings from one rotation seeded at the first param load (0x2: target
 * b->EBX, ours b->EDX; then c/temp ECX<->EBX, n EDX<->EAX, lo/hi rotate).
 * cont.21 levers tried and rejected: register-resident param copy (`bb = b`
 * first-statement AND post-guard both SPLIT b across EDX+ESI, extra push,
 * worse); named `int w = g_cur_player` temp (byte-inert); lo/n statement swap
 * under the new allocator state (byte-inert). Decl order + 3000 cpermute
 * variants on the old form also kept the rotation. Same class as parked
 * 0x26bc8/0x2fbc8. Semantics fully decoded below.
 *
   0x2ed28 -- kill/hit stat bookkeeping. b = pool record with target link at
 * +0x16 (0 = none) and cause flags at +0x1c. n = g_entity_pool + link. The player's
 * own 4 agents occupy pool-A records [g_pool_a + c*0x5c, g_pool_a + (c+4)*0x5c)
 * where c = first-agent index byte g_agent_slots[g_cur_player * 0x417]. If n is an own
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
    unsigned int c;
    unsigned char *lo;
    unsigned char *n;
    unsigned char *hi;

    if (*(unsigned short *)(b + 0x16) == 0)
        return;
    w = g_cur_player;
    c = g_agent_slots[w * 0x417];
    n = g_entity_pool + *(unsigned short *)(b + 0x16);
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
