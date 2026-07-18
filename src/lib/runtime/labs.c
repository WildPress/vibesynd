/* C runtime: labs @ 0x3aed8 (CLIB3S). long abs value. -d2 forces the leaf frame. */
long labs(long x)
{
    return x < 0 ? -x : x;
}
