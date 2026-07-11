extern int *g_5358;
extern unsigned char *g_10ac0;
int FUN_00033fb8(short x, short y, short z)
{
    $[int row = (y % 0x6000) / 256;
    int col = (x & 0xff00) / 256;$|int col = (x & 0xff00) / 256;
    int row = (y % 0x6000) / 256;$]
    int index = $[col + row * 128$|row * 128 + col$];
    $[unsigned char tile = *(unsigned char *)(g_5358[index] + z / 128);$|unsigned char tile = *(unsigned char *)(z / 128 + g_5358[index]);$|int base = g_5358[index]; unsigned char tile = *(unsigned char *)(base + z / 128);$|int base = g_5358[index]; unsigned char tile = *(unsigned char *)(z / 128 + base);$|int zt = z / 128; unsigned char tile = *(unsigned char *)(g_5358[index] + zt);$|int zt = z / 128; unsigned char tile = *(unsigned char *)(zt + g_5358[index]);$|int base = g_5358[index]; int zt = z / 128; unsigned char tile = *(unsigned char *)(base + zt);$|int zt = z / 128; int base = g_5358[index]; unsigned char tile = *(unsigned char *)(zt + base);$]
    unsigned char type = g_10ac0[tile];
    if ((type >= 6 && type <= 9) || type == 0xb)
        return 1;
    return 0;
}
