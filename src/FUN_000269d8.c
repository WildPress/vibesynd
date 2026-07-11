/* frameless @ 0x269d8: advance a linked index through a table of 8-byte records.
   g_5338 points to the record array; p[0x10] is the current record index. Set p[0x10]
   to the record's "next" field (u16 at record+6), then return bit0 of the next
   record's flag byte (record+5).

   PARKED near-miss (NOT matched). The target does two things NO available flag combo
   reproduces together: it caches g_5338 once in EBX (needs -oa alias relaxation) AND
   materialises each element address via `LEA idx*8; ADD ebx; [+disp]` instead of
   folding into SIB `[ebx+idx*8+disp]`. Every -oa-family recipe (-oneatx 42B, -oat 48B,
   -oa 44B) FOLDS; every unfolding recipe (-ot/-or/-oi/no-opt, ~56-57B) reloads g_5338
   twice (no caching). Likely a Watcom minor-version codegen difference. Kept as a
   documented recipe-wall near-miss. */
extern unsigned char *g_5338;
unsigned short FUN_000269d8(unsigned char *p)
{
    *(unsigned short *)(p + 0x10) =
        *(unsigned short *)(g_5338 + (unsigned)*(unsigned short *)(p + 0x10) * 8 + 6);
    return g_5338[(unsigned)*(unsigned short *)(p + 0x10) * 8 + 5] & 1;
}
