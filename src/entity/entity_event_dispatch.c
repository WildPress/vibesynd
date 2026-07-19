/* entity_event_dispatch @ 0x2dd48 - entity event/flag-word (+0xc) dispatcher.
 * Reads the 16-bit flag word at +0xc; if any bit is set it clears the word and
 * returns a code for the first matching bit (each also OR-ing bit3 into flags
 * +0xa): 0x80->0x15, 0x08->0x14, 0x40->0x12, 0x01->0x13, 0x10->0x11. The 0x400
 * bit spawns a sub-object via squad_threat_test(g_entity_pool+node[0x16], p); on success it
 * stamps +0x58=0x1e, +0x20=+0x16, calls func 0xdaa8(0xe, p) and returns 0x2c.
 * Fall-through returns the sub-type byte at +0x19 (zero-extended to ushort).
 * Near-miss (196/203): byte-identical except the flag word lands in EAX (short
 * test al/ah) vs the target's EDX (test dl/dh) + a dead `mov eax,edx` - a pure
 * register tie-break unreachable from C.
 */

extern unsigned char g_entity_pool[];
extern unsigned short squad_threat_test(unsigned char *node, unsigned char *entity);
extern void deliver_entity_event_if_visible(int a, unsigned char *entity);
unsigned short entity_event_dispatch(unsigned char *p)
{
    unsigned short f = *(unsigned short *)(p + 0xc);
    if (f != 0) {
        *(unsigned short *)(p + 0xc) = 0;
        if (f & 0x80) { p[0xa] |= 8; return 0x15; }
        if (f & 0x08) { p[0xa] |= 8; return 0x14; }
        if (f & 0x40) { p[0xa] |= 8; return 0x12; }
        if (f & 0x01) { p[0xa] |= 8; return 0x13; }
        if (f & 0x10) { p[0xa] |= 8; return 0x11; }
        if (f & 0x400) {
            if (squad_threat_test(g_entity_pool + *(unsigned short *)(p + 0x16), p) != 0) {
                p[0x58] = 0x1e;
                p[0xa] |= 8;
                *(unsigned short *)(p + 0x20) = *(unsigned short *)(p + 0x16);
                deliver_entity_event_if_visible(0xe, p);
                return 0x2c;
            }
        }
    }
    return p[0x19];
}
