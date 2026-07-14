/* C runtime: toupper @ 0x3dce5 (CLIB3S). -d2 forces the leaf frame. */
int FUN_0003dce5(int c)
{
    if (c >= 'a' && c <= 'z')
        c -= 0x20;
    return c;
}
