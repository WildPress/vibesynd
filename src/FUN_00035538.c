/* frameless @ 0x35538: bulk dword copy. Copy 15999 dwords from the buffer pointed to
   by g_5370 (src) into the buffer pointed to by g_5368 (dst), loop unrolled x3
   (15999 = 5333 * 3). Mirror of 0x35588 (which copies the other direction).
   Declaration order n, dst, src (n unsigned) reproduces the target's load order
   (count in EBX, dst in EDX, src in EAX) -- found by the permuter's statement reorder. */
extern unsigned int *g_5368;
extern unsigned int *g_5370;
void FUN_00035538(void)
{
    unsigned n = 15999;
    unsigned int *dst = g_5368;
    unsigned int *src = g_5370;
    do {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        n -= 3;
    } while (n != 0);
}
