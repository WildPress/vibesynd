/* 0x12ca8 -- session init. Clears bit 4 of byte +6 in each of 0x80 records of
 * 0xf bytes at 0x1beb2 (compiler-unrolled x4 -- write the PLAIN loop, -oneatx unrolls small do-whiles x4 by itself and DOUBLES any hand-unrolling; mask 0xef hoisted to CL); copies
 * the 4 header bytes 0x1beaa..ad to 0x3eb4..b7 and zeroes the dwords 0x3eb8 /
 * 0x3ebc; stamps 0xd into byte 0 of the 0x40 20-byte records at 0xc36b; then
 * for every player k < g_3eb4 except self (g_cur_player) with player-rec flag 2
 * (g_player_flags[k*0x417]) calls 0x12da8(k). Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char g_1beb2[];
extern unsigned char g_1beaa;
extern unsigned char g_1beab;
extern unsigned char g_1beac;
extern unsigned char g_1bead;
extern unsigned char g_3eb4;
extern unsigned char g_3eb5;
extern unsigned char g_3eb6;
extern unsigned char g_3eb7;
extern int g_3eb8;
extern int g_3ebc;
extern unsigned char g_c36b[];
extern short g_cur_player;
extern unsigned char g_player_flags[];
extern void FUN_00012da8(unsigned char k);

void FUN_00012ca8(void)
{
    unsigned char *p = g_1beb2;
    unsigned short i = 0;
    unsigned short j;
    unsigned short k;

    do {
        p[6] &= 0xef;
        p += 0xf;
        i++;
    } while (i < 0x80);
    g_3eb4 = g_1beaa;
    g_3eb8 = 0;
    g_3eb5 = g_1beab;
    g_3ebc = 0;
    g_3eb6 = g_1beac;
    g_3eb7 = g_1bead;
    for (j = 0; j < 0x40; j++)
        g_c36b[j * 20] = 0xd;
    for (k = 0; k < g_3eb4; k++) {
        if (k != g_cur_player && (g_player_flags[k * 0x417] & 2))
            FUN_00012da8((unsigned char)k);
    }
}
