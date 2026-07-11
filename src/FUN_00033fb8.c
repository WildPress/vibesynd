/* frameless map-passability check @ 0x33fb8 */
extern int *g_5358;
extern unsigned char *g_10ac0;
int FUN_00033fb8(short x, short y, short z)
{
    int row = (y % 0x6000) / 256;
    int col = (x & 0xff00) / 256;
    int index = col + row * 128;
    unsigned char tile = *(unsigned char *)(g_5358[index] + z / 128);
    unsigned char type = g_10ac0[tile];
    if ((type >= 6 && type <= 9) || type == 0xb)
        return 1;
    return 0;
}
