/* frameless @ 0x32118: for block param_2, look up the base record index in the
   0x417-stride block table g_agent_slots, then scan records [idx .. idx+4) in the
   0x5c-stride record table g_108110. For each record with flag (+0x1d & 4) set
   and (+0xb & 1) clear, call los_trace(param_1, rec, param_3); if it echoes
   the record back, return it. Return 0 if none match. */
extern unsigned char g_agent_slots[];
extern unsigned char g_108110[];
extern unsigned char *los_trace(void *param_1, unsigned char *rec, int param_3);

unsigned char *lookup_base_record(void *param_1, unsigned short param_2, short param_3)
{
    unsigned char *rec;
    for (rec = g_108110 + g_agent_slots[param_2 * 0x417] * 0x5c;
         rec < g_108110 + (g_agent_slots[param_2 * 0x417] + 4) * 0x5c;
         rec += 0x5c) {
        if ((rec[0x1d] & 4) && !(rec[0xb] & 1)) {
            if (los_trace(param_1, rec, param_3) == rec)
                return rec;
        }
    }
    return 0;
}
