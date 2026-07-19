/* frameless @ 0x269d8: advance a linked index through a table of 8-byte records.
   g_rec8_table points to the record array; p[0x10] is the current record index (u16).
   Set p[0x10] to the record's "next" field (u16 at record+6), then return bit0 of
   the (now-current) record's flag byte (record+5). The index is re-read from memory
   each time (no CSE) exactly as the target does.

   PARKED near-miss (NOT matched) — register-role + SIB-fold wall (see playbook §3).
   Two irreducible codegen differences vs our Watcom 9.5b, neither source-reachable:
     1. Register roles: target keeps p in volatile EDX and caches the global g_rec8_table
        in the callee-saved EBX; our 9.5b always gives EBX to p (4 refs > g_rec8_table's 2)
        and puts g_rec8_table in a scratch reg. First diff is always at byte 0x2
        (target mov edx,[esp+8] = ..54.. vs ours mov ebx,[esp+8] = ..5c..).
     2. Address form: target UNFOLDS each element address as
        `lea eax,[eax*8+0]; add eax,ebx; mov ..,[eax+disp]`; our 9.5b FOLDS it into a
        single SIB `[base+idx*8+disp]`.
   Tried: direct SIB, materialised record pointer, `base=g_rec8_table` local, explicit
   `i = idx*8` scaled-index temp — all fold to SIB (41-42B) and keep p in EBX.
   Prior sweep also tried -oa/-oat/-oneatx (all fold) and -ot/-or/-oi/-od (all reload
   g_rec8_table twice, no caching). Consistent with a Watcom minor-version codegen
   preference; documented recipe-wall near-miss. */
extern unsigned char *g_rec8_table;
unsigned short advance_linked_index(unsigned char *p)
{
    *(unsigned short *)(p + 0x10) =
        *(unsigned short *)(g_rec8_table + (unsigned)*(unsigned short *)(p + 0x10) * 8 + 6);
    return g_rec8_table[(unsigned)*(unsigned short *)(p + 0x10) * 8 + 5] & 1;
}
