/* 0x2d228 -- animation tick. Increments the frame counter b[0x53] (wraps &7
 * while b[0x49] < 10); if the stage word w14 is 0..0xf and flag 2 set on
 * +0x1c, advances w14 when the counter hits the cadence picked by bits 5-6 of
 * the control word w3c (1: &0x7f, 2: &0x3f, 3: &0x1f). Then updates the three
 * channel triples (+0x49/47/48/4a, +0x4d/4b/4c/4e, +0x51/4f/50/52) via
 * 0x2d0d8, each with its 2-bit mode from w3c (>>7, >>9, >>11).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern void FUN_0002d0d8(unsigned char *p, unsigned char *q, unsigned char *r,
                         int a, int b, int c);

void FUN_0002d228(unsigned char *b)
{

    b[0x53]++;
    if (b[0x49] < 0xa)
        b[0x53] &= 7;
    if (*(short *)(b + 0x14) < 0x10 && *(short *)(b + 0x14) >= 0
        && (b[0x1c] & 2)) {
        switch ((unsigned short)(*(unsigned short *)(b + 0x3c) & 0x60) >> 5) {
        case 1:
            if (!(b[0x53] & 0x7f))
                *(short *)(b + 0x14) = *(short *)(b + 0x14) + 1;
            break;
        case 2:
            if (!(b[0x53] & 0x3f))
                *(short *)(b + 0x14) = *(short *)(b + 0x14) + 1;
            break;
        case 3:
            if (!(b[0x53] & 0x1f))
                *(short *)(b + 0x14) = *(short *)(b + 0x14) + 1;
            break;
        }
    }
    FUN_0002d0d8(b + 0x49, b + 0x47, b + 0x48, b[0x4a], b[0x53],
                 (unsigned char)((unsigned short)(*(unsigned short *)(b + 0x3c) & 0x180) >> 7));
    FUN_0002d0d8(b + 0x4d, b + 0x4b, b + 0x4c, b[0x4e], b[0x53],
                 (unsigned char)((unsigned short)(*(unsigned short *)(b + 0x3c) & 0x600) >> 9));
    FUN_0002d0d8(b + 0x51, b + 0x4f, b + 0x50, b[0x52], b[0x53],
                 (unsigned char)((unsigned short)(*(unsigned short *)(b + 0x3c) & 0x1800) >> 11));
}
