/* frameless @ 0x165f8: count how many of the 50 ten-byte records at g_syndicate_owner have
   their leading byte equal to param. Sibling of 0x16638. Returns the count. */
extern unsigned char g_syndicate_owner[];
unsigned char count_syndicate_recs(unsigned char val)
{
    unsigned char count = 0;
    unsigned char i = 0;
    do {
        if (val == g_syndicate_owner[i * 10])
            count++;
        i++;
    } while (i < 0x32);
    return count;
}
