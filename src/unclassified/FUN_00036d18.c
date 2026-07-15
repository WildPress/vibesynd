/* flag-driven HP adjust @ 0x00036d18 (135B): early-out on flags, subtract from
 * +0x14 word per flag bit in +0xc, call 0x36c78, then mask +0xc word to bit9.
 * NEAR-MISS: register-role wall (0x34048 class). Structure is instruction-exact,
 * but Watcom caches the flag byte in AL (a8 TEST) where the target uses DL
 * (f6 c2 TEST); this cascades into the relative-jump displacements. A word-typed
 * cache reaches DL but only by hoisting/merging the +0xc load (spurious mov,
 * loses the CMP-mem-0 top) -- not the target's shape. */
extern void pool_chain_reset(int p);

void FUN_00036d18(int p)
{
    unsigned char b;

    if (*(unsigned char *)(p + 0xb) & 1)
        return;
    if (*(short *)(p + 0xc) == 0)
        return;
    if (*(short *)(p + 0x1c) != *(short *)(p + 0x16)) {
        b = *(unsigned char *)(p + 0xc);
        if (b & 8) {
            *(short *)(p + 0x14) -= 10;
            pool_chain_reset(p);
        } else if (b & 0x10) {
            *(short *)(p + 0x14) -= 300;
            pool_chain_reset(p);
        } else if (b & 0x80) {
            *(short *)(p + 0x14) -= 300;
            pool_chain_reset(p);
        } else if (b & 0x40) {
            pool_chain_reset(p);
        }
    }
    *(unsigned short *)(p + 0xc) &= 0x200;
}
