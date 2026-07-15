/* frameless @ 0x35588: bulk dword copy, mirror of 0x35538. Copy 15999 dwords from the
   buffer pointed to by g_screen_buf (src) into the buffer pointed to by g_back_buf (dst), loop
   unrolled x3. Same declaration-order trick (n, dst, src) as its sibling. */
extern unsigned int *g_screen_buf;
extern unsigned int *g_back_buf;
void bulk_dword_copy_b(void)
{
    unsigned n = 15999;
    unsigned int *dst = g_back_buf;
    unsigned int *src = g_screen_buf;
    do {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        n -= 3;
    } while (n != 0);
}
