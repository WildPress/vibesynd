/* NEAR-MISS @ 0x2e408 -- 222/240; PARKED on a register-role tie-break. The
 * g_ab60/g_ad60 direction block (byte-identical idiom to the MATCHED 0x2d6c8,
 * same C) allocates the y-product to ESI here instead of the target's ECX
 * (w6 then lands in ECX instead of ESI), cascading ~18 bytes. Same entry
 * context (ebx=p, edi=i, d memory-homed); do-while/for restructure, operand
 * commutes and 2000 cpermute variants all keep the rotated form -- the
 * do-while loop join changes Watcom's internal value numbering vs 0x2d6c8's
 * straight-line body. Guards already share one return-0 stub (|| form).
 *
   0x2e408 -- homing step. Tries up to 4 headings d = dir+step, d-step, ...
 * probing 0x2d468 with the g_ab60/g_ad60 direction step (the 0x2d6c8 idiom).
 * Then if the fresh target angle 0x14c58(dx,dy) is (unsigned) closer than the
 * stored w1e, requires d to be one of the two allowed turns from the launch
 * quadrant p[0x5a] (p[0x5a]-step or p[0x5a]+0x80) else gives up. Decrements
 * the fuel word w42 (0 = give up) and requires turn budget p[0x59] != 0;
 * d == current facing refills p[0x59]=4 else spends one. Commits p[0x1a]=d,
 * returns 1 (0 = remove me). d is a memory-homed char local; i is signed
 * short in EDI (jl). Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_ab60[];
extern short g_ad60[];
extern int FUN_0002d468(int x, int y, int z, unsigned char *obj);
extern short FUN_00014c58(int dx, int dy);

int FUN_0002e408(unsigned char *p, char step)
{
    char d = p[0x1a] + step;
    short i = 0;

    do {
        if ((short)FUN_0002d468(
                (short)((g_ab60[(unsigned char)d] * p[0x54] >> 8) + *(short *)(p + 4)),
                (short)((g_ad60[(unsigned char)d] * p[0x54] >> 8) + *(short *)(p + 6)),
                *(short *)(p + 8), p) != 0)
            break;
        d -= step;
        i++;
    } while (i < 4);
    if ((unsigned short)FUN_00014c58(
            (short)(*(short *)(p + 0x2e) - *(short *)(p + 4)),
            (short)(*(short *)(p + 0x30) - *(short *)(p + 6)))
        < *(unsigned short *)(p + 0x1e)) {
        if ((char)(p[0x5a] - step) != d && (char)(p[0x5a] + 0x80) != d)
            return 0;
    }
    if (--*(unsigned short *)(p + 0x42) == 0 || p[0x59] == 0)
        return 0;
    if (d != p[0x1a])
        p[0x59]--;
    else
        p[0x59] = 4;
    p[0x1a] = d;
    return 1;
}
