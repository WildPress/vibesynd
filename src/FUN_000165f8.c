/* frameless @ 0x165f8: count how many of the 50 ten-byte records at g_539e have
   their leading byte equal to param. Sibling of 0x16638. Returns the count. */
extern unsigned char g_539e[];
unsigned char FUN_000165f8(unsigned char val)
{
    unsigned char count = 0;
    unsigned char i = 0;
    do {
        if (val == g_539e[i * 10])
            count++;
        i++;
    } while (i < 0x32);
    return count;
}
