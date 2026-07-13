/* frameless @ 0x33568: commit funding entry, 866 bytes.
   Returns 2 if g_5595==0; 0 if g_5597==-1; 3 if price g_b95c[g_5597]*g_559b
   exceeds the money int at 0xe49c+g_10b16*0x417 (else deducts it); finds the
   first zero cell of the 10x24 short table g_559d, writes (g_5597+1)*10 plus
   the previous cell (row above's last column when col==0); returns 0 if the
   new value < 0x960, else 1, and on switch(g_5595) case 1/2 stamps 0x960 into
   bank g_5596 of the 0x5788/0x7c05 arrays (strides 0x1eb/0x1f5), copies the
   table in (same loop spelling as sibling FUN_000338d8), and clears a -1
   status in the bank named by the link byte at +0x1e2. */
extern unsigned char g_5595;
extern unsigned char g_5596;
extern int g_5597;
extern unsigned short g_559b;
extern short g_559d[10][24];
extern unsigned short g_b95c[];
extern short g_10b16;
extern unsigned char g_e49c[];
extern unsigned char g_5788[];
extern unsigned char g_578a[];
extern unsigned char g_596a[];
extern unsigned char g_7c05[];
extern unsigned char g_7c07[];
extern unsigned char g_7de7[];

unsigned char FUN_00033568(void)
{
    unsigned char ret;
    unsigned char found, r, c;

    ret = 2;
    if (g_5595 == 0)
        goto done;
    if (g_5597 == -1)
        goto ret0;
    {
        unsigned int cost = g_559b * g_b95c[g_5597];
        if (cost > *(unsigned int *)(g_e49c + g_10b16 * 0x417))
            goto ret3;
        *(unsigned int *)(g_e49c + g_10b16 * 0x417) -= cost;
    }
    found = 0;
    for (r = 0; r < 10; r++) {
        for (c = 0; c < 24; c++) {
            if (g_559d[r][c] == 0) {
                found++;
                break;
            }
        }
        if (found)
            break;
    }
    if (r == 0 && c == 0) {
        g_559d[r][c] = ((unsigned short)g_5597 + 1) * 10;
    } else {
        int v = ((unsigned short)g_5597 + 1) * 10;
        if (c != 0)
            v += g_559d[r][c - 1];
        else
            v += g_559d[r - 1][23];
        g_559d[r][c] = v;
    }
    ret = 0;
    if (g_559d[r][c] < 0x960)
        goto done;
    ret = 1;
    switch (g_5595) {
    case 1: {
        unsigned char rr, cc;
        *(short *)(g_5788 + g_5596 * 0x1eb) = 0x960;
        for (rr = 0; rr < 10; rr++)
            for (cc = 0; cc < 24; cc++)
                *(short *)(g_578a + g_5596 * 0x1eb + rr * 48 + cc * 2) = g_559d[rr][cc];
        if (g_596a[g_5596 * 0x1eb] != 0) {
            if (*(short *)(g_5788 + g_596a[g_5596 * 0x1eb] * 0x1eb) == -1)
                *(short *)(g_5788 + g_596a[g_5596 * 0x1eb] * 0x1eb) = 0;
        }
        break;
    }
    case 2: {
        unsigned char cc, rr;
        *(short *)(g_7c05 + g_5596 * 0x1f5) = 0x960;
        for (rr = 0; rr < 10; rr++)
            for (cc = 0; cc < 24; cc++)
                *(short *)(g_7c07 + g_5596 * 0x1f5 + rr * 48 + cc * 2) = g_559d[rr][cc];
        if (g_7de7[g_5596 * 0x1f5] != 0) {
            if (*(short *)(g_7c05 + g_7de7[g_5596 * 0x1f5] * 0x1f5) == -1)
                *(short *)(g_7c05 + g_7de7[g_5596 * 0x1f5] * 0x1f5) = 0;
        }
        break;
    }
    }
    goto done;
ret3:
    ret = 3;
    goto done;
ret0:
    ret = 0;
done:
    return ret;
}
