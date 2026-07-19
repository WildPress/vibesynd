/* prefix @ 0xfee8 -- previously-undecoded render-path prefix fn (obj1_full, the
 * [0xd748,0x10000) region linear.bin lacks). Copy-if-set: if p1's short at +0x1c
 * is nonzero, store it into p2's short at +0x16. Frameless, stack-calling
 * ([esp+4]/[esp+8]), no relocations. Recipe: -4s -oneatx -zp8 -s -zq */
void copy_if_set(unsigned char *p1, unsigned char *p2)
{
    if (*(short *)(p1 + 0x1c) != 0)
        *(short *)(p2 + 0x16) = *(short *)(p1 + 0x1c);
}
