/* frameless @ 0x355d8: blit the back buffer (g_5368) to VGA memory at 0xa0000, 16000
   dwords, loop unrolled x5 (16000 = 3200 * 5). Cousin of 0x35538/0x35588. Declaration
   order src, dst, n reproduces the target's register/load assignment (permuter). */
extern unsigned int *g_5368;
void FUN_000355d8(void)
{
    unsigned int *src = g_5368;
    unsigned int *dst = (unsigned int *)0xa0000;
    unsigned n = 16000;
    do {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        n -= 5;
    } while (n != 0);
}
