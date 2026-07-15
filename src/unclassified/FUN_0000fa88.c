/* prefix @ 0xfa88 -- Z-probe (twin of 0xfa18, callee FUN_0000fd38). Try the passability test at
 * z+0x7f, z-1, z-0x81; return first nonzero, else a floor-masked z. Frameless, stack-calling.
 * Recipe: -4s -oneatx -zp8 -s -zq. Expected near-miss (same b+1 lea/mov+inc tie as 0xfa18). */
extern unsigned FUN_0000fd38(int x, int y, int z);

unsigned FUN_0000fa88(short x, short y, int z)
{
    int b = z + 0x7f;
    unsigned r = FUN_0000fd38(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    r = FUN_0000fd38(x, y, (short)b);
    if ((short)r != 0)
        return r;
    b -= 0x80;
    if ((short)b < 0)
        return (unsigned)(b + 1) & 0xffffff80;
    r = FUN_0000fd38(x, y, (short)b);
    if ((short)r != 0)
        return r;
    return (unsigned)b & 0xffffff80;
}
