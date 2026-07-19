/* 0x2de18 -- launch setup. Rounds facing to a quadrant (d = ((dir+0x20)>>6<<6)
 * - 0x40), then tries up to 4 quadrants clockwise until the step probe 0x2d6c8
 * (matched twin) accepts. Sets speed p[0x59]=4, aim angle w1e = 0x14c58(dx,dy)
 * from target coords w2e/w30 vs own w4/w6, stores quadrant to p[0x5a], target
 * id to w42. Then a 4-case compare-tree switch on d (0/0x40/0x80/0xc0): pick
 * anim 0xe (turn +0x40) or 0xf (turn -0x40) by comparing d to the raw facing
 * (signed char compare in case 0, unsigned in the others). d is a memory-homed
 * char local. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned short aim_step(unsigned char *obj, unsigned char dir);
extern short sum_of_squares_call(int dx, int dy);

void launch_move_setup(unsigned char *p, int tgt)
{
    char d = ((p[0x1a] + 0x20) >> 6 << 6) - 0x40;
    unsigned short i = 0;

    do {
        if (aim_step(p, d) != 0)
            break;
        d += 0x40;
        i++;
    } while (i < 4);
    p[0x59] = 4;
    *(short *)(p + 0x1e) = sum_of_squares_call(
        (short)(*(short *)(p + 0x2e) - *(short *)(p + 4)),
        (short)(*(short *)(p + 0x30) - *(short *)(p + 6)));
    p[0x5a] = d;
    *(short *)(p + 0x42) = tgt;
    switch (d) {
    case 0:
        if ((signed char)d - (signed char)p[0x1a] < 0) {
            p[0x19] = 0xe;
            p[0x1a] = d + 0x40;
        } else {
            p[0x19] = 0xf;
            p[0x1a] = d - 0x40;
        }
        break;
    case 0x40:
    case 0x80:
    case 0xc0:
        if (d - p[0x1a] < 0) {
            p[0x19] = 0xe;
            p[0x1a] = d + 0x40;
        } else {
            p[0x19] = 0xf;
            p[0x1a] = d - 0x40;
        }
        break;
    }
}
