/* frameless @ 0x14cc8: scan up to 4 records at g_pool_a (stride 0x5c). While a
   record's flag byte [+0xb] has bit 0 set, advance; return 1 on the first
   record whose bit is clear, 0 if all 4 have it set. */
extern unsigned char g_pool_a[];
char scan_pool_4(void)
{
    unsigned char *rec = g_pool_a;
    unsigned char i = 0;
    for (;;) {
        if (!(rec[0xb] & 1))
            return 1;
        i++;
        rec += 0x5c;
        if (i >= 4)
            return 0;
    }
}
