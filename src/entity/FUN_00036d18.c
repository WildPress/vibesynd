/* frameless @ 0x36d18: per-frame update for a pool entity (p = entity record).
   Skips if the "done" flag p[0xb]&1 is set or the state word p[0xc] is zero. When the
   entity's current tile p[0x1c] differs from its target p[0x16], nudge the sub-tile
   position p[0x14] down by a per-mode step (mode bits in the low byte of p[0xc]) and
   run pool_chain_reset(p). Finally collapse the state word to just its 0x0200 bit.
   MATCH lever: the mode bits are tested by re-reading p[0xc] inline in each else-if
   (not via a named temp). Watcom CSEs the four reads into one `mov dl,[ebx+0xc]` in
   dl, matching the original; a named `unsigned char f` instead lands the byte in al
   (short `test al,imm` form) and misses by 4 bytes. */
extern void pool_chain_reset(unsigned char *p);

void flag_hp_adjust(unsigned char *p)
{
    if (p[0xb] & 1)
        return;
    if (*(unsigned short *)(p + 0xc) == 0)
        return;
    if (*(unsigned short *)(p + 0x1c) != *(unsigned short *)(p + 0x16)) {
        if (p[0xc] & 8) {
            *(unsigned short *)(p + 0x14) -= 0xa;
            pool_chain_reset(p);
        } else if (p[0xc] & 0x10) {
            *(unsigned short *)(p + 0x14) -= 0x12c;
            pool_chain_reset(p);
        } else if (p[0xc] & 0x80) {
            *(unsigned short *)(p + 0x14) -= 0x12c;
            pool_chain_reset(p);
        } else if (p[0xc] & 0x40) {
            pool_chain_reset(p);
        }
    }
    *(unsigned short *)(p + 0xc) &= 0x0200;
}
