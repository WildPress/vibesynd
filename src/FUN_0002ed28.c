/* 0x2ed28 -- kill/hit stat bookkeeping. b = pool record with target link at
 * +0x16 (0 = none) and cause flags at +0x1c. n = g_810e + link. The player's
 * own 4 agents occupy pool-A records [g_8110 + c*0x5c, g_8110 + (c+4)*0x5c)
 * where c = first-agent index byte g_e551[g_10b16 * 0x417]. If n is an own
 * agent, bump a counter by cause: &1 g_10af4, &2 g_10afa (shooter also own
 * agent) / g_10af8 (foreign), &0x10 g_10af7, &4 g_10af5, &8 g_10af6. Then if
 * n's flags have 1|2|0x10: either mark n[0x1d]|=2 (b is &4 and n isn't) or
 * n[0x1c]|=0x40. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_10b16;
extern unsigned char g_e551[];
extern unsigned char g_810e[];
extern unsigned char g_8110[];
extern unsigned char g_10af4;
extern unsigned char g_10af5;
extern unsigned char g_10af6;
extern unsigned char g_10af7;
extern unsigned char g_10af8;
extern unsigned char g_10afa;

void FUN_0002ed28(unsigned char *b)
{
    unsigned char c;
    unsigned char *lo;
    unsigned char *n;
    unsigned char *hi;

    if (*(unsigned short *)(b + 0x16) == 0)
        return;
    c = g_e551[g_10b16 * 0x417];
    lo = g_8110 + c * 0x5c;
    n = g_810e + *(unsigned short *)(b + 0x16);
    if (n >= lo) {
        hi = g_8110 + (c + 4) * 0x5c;
        if (n < hi) {
            if (b[0x1c] & 1)
                g_10af4++;
            else if (b[0x1c] & 2) {
                if (b >= lo && b < hi)
                    g_10afa++;
                else
                    g_10af8++;
            } else if (b[0x1c] & 0x10)
                g_10af7++;
            else if (b[0x1c] & 4)
                g_10af5++;
            else if (b[0x1c] & 8)
                g_10af6++;
        }
    }
    if ((n[0x1c] & 1) || (n[0x1c] & 2) || (n[0x1c] & 0x10)) {
        if ((b[0x1c] & 4) && !(n[0x1c] & 4)) {
            n[0x1d] |= 2;
            return;
        }
        n[0x1c] |= 0x40;
    }
}
