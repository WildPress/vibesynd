/* frameless @ 0x33568: commit funding entry, 866 bytes (manifest size correct).
   Returns 2 if g_5595==0; 0 if g_5597==-1; 3 if price g_b95c[g_5597]*g_559b
   exceeds the money int at 0xe49c+g_10b16*0x417 (else deducts it); finds the
   first zero cell of the 10x24 short table g_559d, writes (g_5597+1)*10 plus
   the previous cell (row above's last column when col==0); returns 0 if the
   new value < 0x960, else 1, and on switch(g_5595) case 1/2 stamps 0x960 into
   bank g_5596 of the 0x5788/0x7c05 arrays (strides 0x1eb/0x1f5), copies the
   table in (same loop spelling as sibling FUN_000338d8), and clears a -1
   status in the bank named by the link byte at +0x1e2.

   🅿️ PARKED 864/866, ~161 diff bytes over 10 recipes/spellings (default
   -4s -oneatx -zp8 -s -zq; -or is way off at 772).
   MATCHED regions: entry+ret=2 guard, g_5597==-1 far-jump, cost multiply
   (g_b95c[i]*g_559b spelling, price and-form in EAX / g_559b xor-form in EBX),
   money deduct, whole 10x24 zero-scan (found/bh/bl registerised), else-arm
   base init (mov si;inc esi;imul esi,esi,0xa), arm1 ([r][c-1], canonical
   89c2/31c0 chain + add esi,ecx), case1 header+copy loop+link-byte tail
   incl. 31c9 zero store, all four counter spill slots (r1@0,r2@4,c1@8,c2@0xc
   via decl order r1,r2,c2,c1 — note: cols honored REVERSED decl order).
   WALL: anti-correlated register-role parity across ~5 sites. Exactly one of
   {arm1, arm2} and one of {case1, case2} inner-index chains compiles to the
   canonical `mov edx,eax; xor eax,eax; shl edx,4` form per build; the sibling
   always flips to the `xor edx,edx; shl eax,4` form ([eax+edx*2] modrm), which
   also kills the target's arm1->arm2 store cross-jump (ours duplicates the
   8-byte store + eb36). Every addend-order/decl/alias/deref permutation tried
   (attempts 3-10) flips WHICH site is canonical, never makes both. Remaining
   diffs: arm2 rotation (r widen picks ECX not EDX), case2 inner chain flip,
   post-store re-read chain flip + [eax+edx*2] cmp load, then-branch value
   route (ours mov cx,[g_5597](8b0d) direct vs target mov ax(66a1);inc eax;
   imul ecx,eax,0xa — named temp/alias/deref/commute all failed), ret0 block
   zero reg (ours xor bl,bl vs target xor dh,dh), and alignment-pad encodings
   that will self-heal once sizes align. Same family as the 0x338d8 park. */
extern unsigned char g_5595;
extern unsigned char g_5596;
extern int g_5597;
extern unsigned short g_5597w; /* word alias of g_5597 (fixups masked) */
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
    unsigned char r1, r2, c2, c1;

    ret = 2;
    if (g_5595 == 0)
        goto done;
    if (g_5597 == -1)
        goto ret0;
    {
        unsigned int cost = g_b95c[g_5597] * g_559b;
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
        g_559d[r][c] = (*(unsigned short *)&g_5597 + 1) * 10;
    } else {
        unsigned short base = ((unsigned short)g_5597 + 1) * 10;
        if (c != 0)
            g_559d[r][c] = g_559d[r][c - 1] + base;
        else
            g_559d[r][c] = g_559d[r - 1][23] + base;
    }
    ret = 0;
    if (g_559d[r][c] < 0x960)
        goto done;
    ret = 1;
    switch (g_5595) {
    case 1: {
        *(short *)(g_5788 + g_5596 * 0x1eb) = 0x960;
        for (r1 = 0; r1 < 10; r1++)
            for (c1 = 0; c1 < 24; c1++)
                *(short *)(g_578a + g_5596 * 0x1eb + r1 * 48 + c1 * 2) = g_559d[r1][c1];
        if (g_596a[g_5596 * 0x1eb] != 0) {
            if (*(short *)(g_5788 + g_596a[g_5596 * 0x1eb] * 0x1eb) == -1)
                *(short *)(g_5788 + g_596a[g_5596 * 0x1eb] * 0x1eb) = 0;
        }
        break;
    }
    case 2: {
        *(short *)(g_7c05 + g_5596 * 0x1f5) = 0x960;
        for (r2 = 0; r2 < 10; r2++)
            for (c2 = 0; c2 < 24; c2++)
                *(short *)(g_7c07 + g_5596 * 0x1f5 + r2 * 48 + c2 * 2) = g_559d[r2][c2];
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
