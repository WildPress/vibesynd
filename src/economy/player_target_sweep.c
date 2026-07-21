/* 0x164c8 -- per-player target (re)assignment sweep. For each player slot i<8:
 * resets g_e13c[i]=0xff, skips self (g_cur_player); x=FUN_165f8(i) rates the slot
 * (0x32 aborts the whole sweep, 0 skips); pick d = x==1 ? 1 : rand(x-1)+1 via
 * FUN_e568, then r=FUN_16638(d,i). Scans the 8 entries of r's 19-byte row in
 * g_syndicate_links: nonzero entry v indexes a 10-byte record g_syndicate_owner[(v-1)*10]; if its
 * owner byte isn't i/self and a d100 roll < 0x32, claims it (owner=i,
 * g_e13c[i]=r) -- first hit ends the row scan. Recipe: -4s -oneatx -zp8 -s -zq
 * Lever: the self-check's 16-bit `cmp dx,bx` with a full `xor edx,edx` widen
 * needs `unsigned int t = i; if ((unsigned short)t == g_cur_player)`. A direct
 * (unsigned short)i cast or a ushort temp gives `xor dh,dh` and lets the
 * allocator share EDX halves with the 0xff store (b2ff reg-store) and flips
 * the stack-slot order; the int temp forbids the half-reg trick.
 */
extern unsigned char g_e13c[];
extern unsigned short g_cur_player;
extern unsigned char g_syndicate_links[];
extern unsigned char g_syndicate_owner[];
extern int count_syndicate_recs(int slot);
extern unsigned short lcg_rand(unsigned short n);
extern int scan_syndicate_recs(int d, int slot);

void player_target_sweep(void)
{
    unsigned char i;
    unsigned char r;
    unsigned char x;
    unsigned char d;
    unsigned char j;
    unsigned int t;

    for (i = 0; i < 8; i++) {
        g_e13c[i] = 0xff;
        t = i;
        if ((unsigned short)t == g_cur_player)
            continue;
        x = count_syndicate_recs(i);
        if (x == 0x32)
            return;
        if (x == 0)
            continue;
        d = x;
        if (x != 1)
            d = lcg_rand(x - 1) + 1;
        r = scan_syndicate_recs(d, i);
        for (j = 0; j < 8; j++) {
            if (g_syndicate_links[j + r * 19] == 0)
                continue;
            if (g_syndicate_owner[(g_syndicate_links[j + r * 19] - 1) * 10] == i)
                continue;
            if ((unsigned short)g_syndicate_owner[(g_syndicate_links[j + r * 19] - 1) * 10] == g_cur_player)
                break;
            if (lcg_rand(0x64) >= 0x32)
                break;
            g_syndicate_owner[(g_syndicate_links[j + r * 19] - 1) * 10] = i;
            g_e13c[i] = r;
            break;
        }
    }
}
