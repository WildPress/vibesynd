/* frameless @ 0x34118: from src->[0x1c] (id into object pool g_entity_pool), take the
   node's byte[0x54]; if it exceeds threshold thr, store (byte-0x54 - thr) into
   dst[0x54], else store 0. Was parked as a "register wall"; cracked with the
   inline idiom — load the id and node[0x54] INLINE (repeated subexpr) and drop
   the (int) casts, so Watcom CSEs id into a persistent reg and re-reads thr from
   the stack at each width (word compare / byte subtract) exactly like the target,
   instead of caching a named var. */
extern unsigned char g_entity_pool[];
void field54_sub_threshold(unsigned char *dst, unsigned char *src, unsigned short thr)
{
    if (*(unsigned short *)(src + 0x1c) != 0) {
        unsigned char *node = g_entity_pool + *(unsigned short *)(src + 0x1c);
        if (node[0x54] > thr) {
            dst[0x54] = node[0x54] - thr;
            return;
        }
    }
    dst[0x54] = 0;
}
