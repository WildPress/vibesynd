/* 0x30508 -- entity update (pool obj in ebx). Sets flag 2 on +0xb; computes

 * calls 0x26ad8((ushort)((e568((ushort)hp) + 2)/2 + hp), (ushort)dir) -- the
 * nested call inside arg1 makes Watcom push arg2 early and split the cleanups
 * 4-then-8; halves hp the same way ((hp+2)/2); 0x2d3b8(p);
 * if g_e128 < -0x40 decrements the short at +0x14; if 0x269d8(p) != 0 either
 * marks type 0x18 (counter went negative) or type 0x16 + dir += 0x80,
 * word +0xa &= ~0x208 (signed short => sign-extended mask + ESI), +0x5b = dir, 0x2d998(p). Finally +0x19 = 0x2dd48(p).
 * The (unsigned short) casts of byte fields give the two-step widen
 * (xor ah,ah / and eax,0xffff). Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned short FUN_LE_0000e568(unsigned short a);
extern void FUN_00026ad8(unsigned short a, unsigned short b);
extern void FUN_0002d3b8(unsigned char *p);
extern short g_e128;
extern unsigned short FUN_000269d8(unsigned char *p);
extern void FUN_0002d998(unsigned char *p);
extern char FUN_0002dd48(unsigned char *p);

void FUN_00030508(unsigned char *p)
{
    p[0xb] |= 2;
    FUN_00026ad8(
        (unsigned short)((FUN_LE_0000e568((unsigned short)p[0x54]) + 2) / 2 + p[0x54]),
        (unsigned short)p[0x1a]);
    p[0x54] = (p[0x54] + 2) / 2;
    FUN_0002d3b8(p);
    if (g_e128 < -0x40)
        (*(short *)(p + 0x14))--;
    if (FUN_000269d8(p) != 0) {
        if (*(short *)(p + 0x14) < 0) {
            p[0x19] = 0x18;
        } else {
            p[0x1a] += 0x80;
            *(short *)(p + 0xa) &= ~0x208;
            p[0x19] = 0x16;
            p[0x5b] = p[0x1a];
            FUN_0002d998(p);
        }
    }
    p[0x19] = FUN_0002dd48(p);
}
