/* frameless @ 0x2ed28: tally/flag an entity relative to the current player's agents.
   The entity is p1->field16 (index into g_entity_pool). The current player's agent block
   is the 4 records [cat, cat+4) of g_pool_a, where cat = g_agent_slots[cur_player*1047].
   If the entity falls in that block, bump one of several per-category counters by p1's
   flag byte (p1[0x1c]). Then, if the entity's own flags (bit 0/1/4) are set, either set
   the entity's +0x1d bit 1 (when p1 is a fresh selection) or its +0x1c bit 0x40.

   PARKED near-miss. Logic + the constant-multiply chains (cur_player*1047, cat*0x5c) and
   all 11 relocations match. Diffs are register-role: the original keeps p1 in ebx and rec
   in edx (we swap them), CSEs the last three flag tests through ah, and emits the final
   `rec[0x1c] |= 0x40` as a direct `or [mem],imm` where we route it through a register.
   None source-reachable here; the p1/rec register choice is a free-allocation tie-break
   (0 calls, both long-lived). Register-role family. */
extern short g_cur_player;
extern unsigned char g_agent_slots[];
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char g_10af4, g_10af5, g_10af6, g_10af7, g_10af8, g_10afa;

void record_kill_stats(unsigned char *p1)
{
    unsigned cat;
    unsigned char *rec, *lo, *hi;
    if (*(unsigned short *)(p1 + 0x16) == 0)
        return;
    cat = g_agent_slots[g_cur_player * 1047];
    lo = g_pool_a + cat * 0x5c;
    rec = g_entity_pool + *(unsigned short *)(p1 + 0x16);
    if (rec >= lo) {
        hi = g_pool_a + (cat + 4) * 0x5c;
        if (rec < hi) {
            if (p1[0x1c] & 1) {
                g_10af4++;
            } else if (p1[0x1c] & 2) {
                if (p1 >= lo && p1 < hi)
                    g_10afa++;
                else
                    g_10af8++;
            } else if (p1[0x1c] & 0x10) {
                g_10af7++;
            } else if (p1[0x1c] & 4) {
                g_10af5++;
            } else if (p1[0x1c] & 8) {
                g_10af6++;
            }
        }
    }
    if ((rec[0x1c] & 1) || (rec[0x1c] & 2) || (rec[0x1c] & 0x10)) {
        if (p1[0x1c] & 4) {
            if (rec[0x1c] & 4)
                rec[0x1c] |= 0x40;
            else
                rec[0x1d] |= 2;
        } else {
            rec[0x1c] |= 0x40;
        }
    }
}
