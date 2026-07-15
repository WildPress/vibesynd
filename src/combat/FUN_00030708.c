/* @ 0x00030708 (142B): step an entity then adjust its HP word (+0x14) and
 *   flags word (+0xa) by a code derived from flags (+0x1d,+0x3c).
 *   call 0x269d8(p); if((short)==0) return;
 *   code<2 -> HP=-1; ==2 -> HP-=8; ==3 -> HP-=4; else unchanged;
 *   HP<0 -> set +0x19=0x19, +0xa|=0x109; else clear +0xa&=0xfdf7, call 0x2d998.
 */
extern unsigned short advance_linked_index(unsigned char *);
extern void           recompute_state_code(unsigned char *);

void FUN_00030708(unsigned char *p)
{
    unsigned short c;

    c = 0;
    if ((short)advance_linked_index(p) == 0)
        return;
    if ((*(unsigned char *)(p + 0x1d) & 0x10) == 0)
        c = (unsigned short)(*(unsigned short *)(p + 0x3c) & 0x60) >> 5;
    if (c < 2)
        goto lo;
    if (c <= 2)
        goto sub8;
    if (c == 3)
        goto sub4;
    goto hp;
lo:
    *(unsigned short *)(p + 0x14) = 0xffff;
    goto set;
sub8:
    *(short *)(p + 0x14) -= 8;
    goto hp;
sub4:
    *(short *)(p + 0x14) -= 4;
hp:
    if (*(short *)(p + 0x14) < 0) {
    set:
        *(unsigned char *)(p + 0x19) = 0x19;
        *(unsigned short *)(p + 0xa) |= 0x109;
        return;
    }
    *(unsigned short *)(p + 0xa) &= ~0x208;
    recompute_state_code(p);
}
