/* 0x139a8 -- kill-credit eligibility test. n's cause flags at +0x1c pick a
 * rule bit in b[1]: &4 -> b[1]&1; &2 -> owner p = ((n - g_8110)/0x5c)/8, if
 * player-rec flag (g_e4aa[p*0x417] & 2) then p==idx forces 0 else b[1]&4,
 * non-player owner checks b[1]&2; &1 -> b[1]&0x10; &8 -> b[1]&0x20; else
 * !( &0x10) or b[1]&0x40. result is a memory-homed char local returned at a
 * shared bottom return (Watcom tail-duplicates several arms).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char g_8110[];
extern unsigned char g_e4aa[];

char FUN_000139a8(unsigned char *b, unsigned char *n, unsigned char idx)
{
    char result = 0;
    int p;

    if (n[0x1c] & 4) {
        if (b[1] & 1)
            result = 1;
    } else if (n[0x1c] & 2) {
        p = (int)((n - g_8110) / 0x5c) / 8;
        if (g_e4aa[p * 0x417] & 2) {
            if (p == idx)
                result = 0;
            else if (b[1] & 4)
                result = 1;
        } else if (b[1] & 2)
            result = 1;
    } else if (n[0x1c] & 1) {
        if (b[1] & 0x10)
            result = 1;
    } else if (n[0x1c] & 8) {
        if (b[1] & 0x20)
            result = 1;
    } else if (!(n[0x1c] & 0x10) || (b[1] & 0x40))
        result = 1;
    return result;
}
