/* frameless @ 0x14cc8: scan up to 4 records at g_8110 (stride 0x5c). While a
   record's flag byte [+0xb] has bit 0 set, advance; return 1 on the first
   record whose bit is clear, 0 if all 4 have it set. */
extern unsigned char g_8110[];
char FUN_00014cc8(void)
{
    unsigned char *rec = g_8110;
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
