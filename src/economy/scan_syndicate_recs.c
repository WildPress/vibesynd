/* frameless @ 0x16638: scan a table of 10-byte records at g_syndicate_owner, wrapping the
   index at 0x32, and return the index of the count-th record whose byte equals
   val. Returns 0xff if count is zero. */
extern unsigned char g_syndicate_owner[];
char scan_syndicate_recs(unsigned char count, unsigned char val)
{
    signed char idx = 0xff;
    while (count != 0) {
        idx++;
        if (idx >= 0x32)
            idx = 0;
        if (val == g_syndicate_owner[idx * 10])
            count--;
    }
    return idx;
}
