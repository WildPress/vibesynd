/* frameless push-ebx @ 0x34168: field p[0x54] (uchar) clamp-decrement by n (u16) */
void clamp_dec_field54(unsigned char *p, unsigned short n)
{
    if (p[0x54] > n)
        p[0x54] -= (unsigned char)n;
    else
        p[0x54] = 0;
}
