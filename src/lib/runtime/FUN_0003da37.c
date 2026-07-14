/* C runtime: tolower @ 0x3da37 (CLIB3S). -d2 forces the leaf frame. */
int FUN_0003da37(int c)
{
    if (c >= 'A' && c <= 'Z')
        c += 0x20;
    return c;
}
