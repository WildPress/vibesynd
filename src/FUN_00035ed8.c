/* frameless @ 0x35ed8: zero 8 byte fields (offsets 0,2,4,...,0xe) of each of 32 records
   (stride 0x10) in the table at g_11bec. Index i steps 0x10 up to 0x200; the store value
   0 is cached in DL across the loop.

   PARKED near-miss (NOT matched). With -oneat the INSTRUCTIONS are byte-correct (63B:
   DL-cached, 8 stores, no unroll). The target is that + 12B of loop-alignment NOP
   padding (two 6-byte `lea`-nops before the loop top). But in this Watcom the loop
   alignment only comes bundled with -ol, and -ol ALSO unrolls this counted loop x2
   (123B). No flag gives align WITHOUT unroll. Same class as 0x269d8 (a Watcom
   loop-opt bundling we can't split). Kept as byte-correct-instructions reference. */
extern unsigned char g_11bec[];
void FUN_00035ed8(void)
{
    int i = 0;
    do {
        g_11bec[i] = 0;
        g_11bec[i + 2] = 0;
        g_11bec[i + 4] = 0;
        g_11bec[i + 6] = 0;
        g_11bec[i + 8] = 0;
        g_11bec[i + 0xa] = 0;
        g_11bec[i + 0xc] = 0;
        g_11bec[i + 0xe] = 0;
        i += 0x10;
    } while (i != 0x200);
}
