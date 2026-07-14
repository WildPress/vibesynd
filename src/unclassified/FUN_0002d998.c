/* frameless status logic @ 0x2d998: recompute p->b58 (a state code) then mirror to p->b19 */
void FUN_0002d998(unsigned char *p)
{
    if (p[0x19] == p[0x58] || p[0x58] == 0) {
        if (*(unsigned short *)(p + 0x20) != 0)
            p[0x58] = 0x1e;
        else if (p[0xb] & 0x10)
            p[0x58] = 0x1d;
        else if (*(unsigned short *)(p + 0x28) != 0)
            p[0x58] = 1;
        else
            p[0x58] = 0;
    }
    p[0x19] = p[0x58];
}
