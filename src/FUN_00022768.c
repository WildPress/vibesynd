/* frameless @ 0x22768: PARKED near-miss (register allocation). Logic is correct;
   with four live pointers (three results + the scan cursor) Watcom assigns EBX/
   ESI to pool A vs C the opposite way round, and no C reshuffle we tried flips
   it. Kept for the structure it documents.

   For each of three CONTIGUOUS object pools, scan from the top downward while
   slots are free (the byte at record+0x18 is the "in use" flag) and record the
   lowest free slot; cache the three pointers in globals. If g_10b42 is set, skip
   the scan and just store the pool ends.
     pool A: [0x8110, 0xdd10)  256 x 0x5c (92B)   -> free ptr in g_10ae0
     pool B: [0xdd10, 0xe790)   64 x 0x2a (42B)   -> free ptr in g_10adc
     pool C: [0xe790, 0x11670) 400 x 0x1e (30B)   -> free ptr in g_10aec */
extern unsigned char pool_a[];    /* 0x8110  */
extern unsigned char pool_b[];    /* 0xdd10  = end of A */
extern unsigned char pool_c[];    /* 0xe790  = end of B */
extern unsigned char pool_end[];  /* 0x11670 = end of C */
extern unsigned char g_10b42;
extern unsigned char *g_10ae0, *g_10adc, *g_10aec;
void FUN_00022768(void)
{
    unsigned char *a = pool_b;
    unsigned char *b = pool_c;
    unsigned char *c = pool_end;
    if (g_10b42 == 0) {
        unsigned char *e;
        for (e = pool_b - 0x5c; e >= pool_a; e -= 0x5c) {
            if (e[0x18] != 0)
                break;
            a = e;
        }
        for (e = pool_c - 0x2a; e >= pool_b; e -= 0x2a) {
            if (e[0x18] != 0)
                break;
            b = e;
        }
        for (e = pool_end - 0x1e; e >= pool_c; e -= 0x1e) {
            if (e[0x18] != 0)
                break;
            c = e;
        }
    }
    g_10aec = c;
    g_10adc = b;
    g_10ae0 = a;
}
