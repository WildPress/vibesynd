/* 0x2def8 (1284B): projectile trajectory aim/turn selection.
 * Compute aim angle w1e = 0x14c58(dx,dy) from target coords obj[0x2e]/obj[0x30]
 * vs own obj[4]/obj[6]. If busy flag g_5300 != 0, bail. Set g_5300 = 1.
 * Round facing to a quadrant d = ((obj[0x1a]+0x20)>>6<<6)-0x40. Search up to 4
 * quadrants clockwise with the two step probes (0x2d738 for dA, 0x2d6c8 for dB).
 * If 0x2d6c8 found none, take the fallback: search again from dA+0x20, add a random
 * +/-0x20 spread, seed the shot cursor (g_aim_x/2a/2c) and commit via 0x26ad8 + 0x2d3b8.
 * Otherwise stamp obj (aim, quadrant dB->0x5a, speed 4->0x59, tgt->0x42) and simulate
 * two candidate trajectories in stack copies of the entity (buf2 turned +0x40, buf1
 * turned -0x40), marching each 32 steps via 0x2e4f8 + a direction-table step, tracking
 * the closest approach (distance via 0x26aa8) and the step at which it occurred. If a
 * copy lands on the target tile mid-march, pick its turn immediately (anim 0xe/0xf).
 * After 32 steps, choose the turn whose trajectory got closer sooner (128 units/step
 * tie-break). Recipe: -4s -oneatx -zp8 -s -zq */
extern short g_5300;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_level_step;
extern short g_aim_x;
extern short g_aim_y;
extern short g_aim_level;

extern int FUN_00014c58(int dx, int dy);
extern int projectile_step(unsigned char *obj, unsigned char dir);
extern unsigned short aim_step(unsigned char *obj, unsigned char dir);
extern int max_abs(short a, short b);
extern void find_projectile_step(unsigned char *obj, char dir);
extern unsigned char lcg_rand(int a);
extern void FUN_00026ad8(unsigned short mult, unsigned short idx);
extern int aim_cursor_advance(int obj);
extern void FUN_0004d1db(void *a, void *b, unsigned int n);

void FUN_0002def8(unsigned char *obj, int tgt)
{
    unsigned char buf1[0x5c];
    unsigned char buf2[0x5c];
    int j;
    unsigned short aim;
    int i;
    unsigned short rng;
    int b1iter;
    int b2iter;
    int b1dist;
    unsigned char dB;
    unsigned char dA;
    int b2dist;

    aim = FUN_00014c58(
        (short)(*(short *)(obj + 0x2e) - *(short *)(obj + 4)),
        (short)(*(short *)(obj + 0x30) - *(short *)(obj + 6)));
    if (g_5300 != 0)
        goto done;
    g_5300 = 1;
    dA = dB = ((obj[0x1a] + 0x20) >> 6 << 6) - 0x40;

    for (j = 0; j < 4; j++) {
        if (projectile_step(obj, dA) != 0)
            break;
        dA += 0x40;
    }
    for (j = 0; j < 4; j++) {
        if (aim_step(obj, dB) != 0)
            break;
        dB += 0x40;
    }
    if (j >= 4)
        goto fallback;

    obj[0x5a] = dB;
    *(short *)(obj + 0x42) = tgt;
    obj[0x59] = 4;
    *(short *)(obj + 0x1e) = aim;

    FUN_0004d1db(obj, buf1, 0x5c);
    buf1[0x1a] = dA - 0x40;
    buf1[0x5a] = dA - 0x40;
    buf1[0x54] = 0x80;
    *(short *)(buf1 + 0x1e) = max_abs(
        (short)(*(short *)(buf1 + 0x2e) - *(short *)(buf1 + 4)),
        (short)(*(short *)(buf1 + 0x30) - *(short *)(buf1 + 6)));

    FUN_0004d1db(obj, buf2, 0x5c);
    buf2[0x1a] = dA + 0x40;
    buf2[0x5a] = dA + 0x40;
    buf2[0x54] = 0x80;
    *(short *)(buf2 + 0x1e) = max_abs(
        (short)(*(short *)(buf2 + 0x2e) - *(short *)(buf2 + 4)),
        (short)(*(short *)(buf2 + 0x30) - *(short *)(buf2 + 6)));

    b2dist = 0;
    b1iter = 0;
    b2iter = 0;
    b1dist = 0;
    obj[0x19] = 0;
    for (i = 0; i < 0x20; i++) {
        find_projectile_step(buf2, -0x40);
        if ((*(short *)(obj + 0x2e) >> 8) == (*(short *)(buf2 + 4) >> 8) &&
            (*(short *)(obj + 0x30) >> 8) == (*(short *)(buf2 + 6) >> 8)) {
            obj[0x19] = 0xe;
            obj[0x1a] = obj[0x5a] + 0x40;
            goto done;
        }
        *(short *)(buf2 + 4) = *(short *)(buf2 + 4) + (g_dir_dx[buf2[0x1a]] * 0x80 >> 8);
        *(short *)(buf2 + 6) = *(short *)(buf2 + 6) + (g_dir_dy[buf2[0x1a]] * 0x80 >> 8);
        *(short *)(buf2 + 8) = *(short *)(buf2 + 8) + g_level_step;
        rng = max_abs(
            (short)(*(short *)(buf2 + 0x2e) - *(short *)(buf2 + 4)),
            (short)(*(short *)(buf2 + 0x30) - *(short *)(buf2 + 6)));
        if (rng < *(unsigned short *)(buf2 + 0x1e)) {
            *(short *)(buf2 + 0x1e) = rng;
            b2dist = rng;
            b2iter = i;
        }

        find_projectile_step(buf1, 0x40);
        if ((*(short *)(obj + 0x2e) >> 8) == (*(short *)(buf1 + 4) >> 8) &&
            (*(short *)(obj + 0x30) >> 8) == (*(short *)(buf1 + 6) >> 8)) {
            obj[0x19] = 0xf;
            obj[0x1a] = obj[0x5a] - 0x40;
            return;
        }
        *(short *)(buf1 + 4) = *(short *)(buf1 + 4) + (g_dir_dx[buf1[0x1a]] * 0x80 >> 8);
        *(short *)(buf1 + 6) = *(short *)(buf1 + 6) + (g_dir_dy[buf1[0x1a]] * 0x80 >> 8);
        *(short *)(buf1 + 8) = *(short *)(buf1 + 8) + g_level_step;
        rng = max_abs(
            (short)(*(short *)(buf1 + 0x2e) - *(short *)(buf1 + 4)),
            (short)(*(short *)(buf1 + 0x30) - *(short *)(buf1 + 6)));
        if (rng < *(unsigned short *)(buf1 + 0x1e)) {
            *(short *)(buf1 + 0x1e) = rng;
            b1dist = rng;
            b1iter = i;
        }
    }

    if (obj[0x19] != 0)
        goto done;
    if (b1iter == 0) {
        obj[0x19] = 0xe;
        obj[0x1a] = obj[0x5a] + 0x40;
        return;
    }
    if (b2iter == 0) {
        obj[0x19] = 0xf;
        obj[0x1a] = obj[0x5a] - 0x40;
        return;
    }
    if (b1iter > b2iter) {
        if (b2dist <= b1dist) {
            obj[0x19] = 0xe;
            obj[0x1a] = obj[0x5a] + 0x40;
            return;
        }
        if ((b1iter - b2iter) * 0x80 >= b2dist - b1dist) {
            obj[0x19] = 0xe;
            obj[0x1a] = obj[0x5a] + 0x40;
            return;
        }
        obj[0x19] = 0xf;
        obj[0x1a] = obj[0x5a] - 0x40;
        return;
    }
    if (b2dist < b1dist) {
        obj[0x19] = 0xf;
        obj[0x1a] = obj[0x5a] - 0x40;
        return;
    }
    if ((b2iter - b1iter) * 0x80 >= b1dist - b2dist) {
        obj[0x19] = 0xf;
        obj[0x1a] = obj[0x5a] - 0x40;
        return;
    }
    obj[0x19] = 0xe;
    obj[0x1a] = obj[0x5a] + 0x40;
    return;

fallback:
    dA += 0x20;
    for (j = 0; j < 4; j++) {
        if (aim_step(obj, dA) != 0)
            break;
        dA += 0x40;
    }
    dA = dA + ((lcg_rand(2) * 2 - 1) << 5);
    g_aim_x = *(short *)(obj + 4);
    g_aim_y = *(short *)(obj + 6);
    g_aim_level = *(short *)(obj + 8);
    obj[0x1a] = dA;
    FUN_00026ad8(obj[0x54], dA);
    aim_cursor_advance((int)obj);
done:
    ;
}
