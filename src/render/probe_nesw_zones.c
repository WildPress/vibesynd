/* 0x28d08 -- probe four adjacent zones. Each enabled flag (short params
 * a2..a5) fires the fixup callee @0xffc8 with (b, x, y, z-0x100, w, h, 0x100,
 * 0x200): N (x+0x100, y+0x240, 0x80x0x100), E (x+0x240, y-0x100,
 * 0x100x0x80), S (x-0x100, y-0x240, 0x80x0x100), W (x-0x240, y+0x100,
 * 0x100x0x80). +0x100 on a word compiles to `inc ah`.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern void FUN_LE_0000ffc8(unsigned char *b, int x, int y, int z,
                            int w, int h, int d, int e);

void probe_nesw_zones(unsigned char *b, short n, short e, short s, short w)
{
    if (n != 0)
        FUN_LE_0000ffc8(b, (short)(*(short *)(b + 4) + 0x100),
                        (short)(*(short *)(b + 6) + 0x240),
                        (short)(*(short *)(b + 8) - 0x100),
                        0x80, 0x100, 0x100, 0x200);
    if (e != 0)
        FUN_LE_0000ffc8(b, (short)(*(short *)(b + 4) + 0x240),
                        (short)(*(short *)(b + 6) - 0x100),
                        (short)(*(short *)(b + 8) - 0x100),
                        0x100, 0x80, 0x100, 0x200);
    if (s != 0)
        FUN_LE_0000ffc8(b, (short)(*(short *)(b + 4) - 0x100),
                        (short)(*(short *)(b + 6) - 0x240),
                        (short)(*(short *)(b + 8) - 0x100),
                        0x80, 0x100, 0x100, 0x200);
    if (w != 0)
        FUN_LE_0000ffc8(b, (short)(*(short *)(b + 4) - 0x240),
                        (short)(*(short *)(b + 6) + 0x100),
                        (short)(*(short *)(b + 8) - 0x100),
                        0x100, 0x80, 0x100, 0x200);
}
