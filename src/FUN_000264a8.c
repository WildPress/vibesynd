/* frameless @ 0x264a8: slot-claim eligibility test for record p (char return).
   Tables: 10-byte records at 0x539c (word at +0 = claim word, byte at +2 = owner)
   and 19-byte rows at 0xb069 (8 link bytes at +0, flag at +8) â€” same pair 0x164c8
   walks. save = g_10b16 (self), restored at the single exit (Watcom tail-
   duplicates the store+ret into the branches). Returns 1 if: g_539a set; or
   row-flag[p]==1 and (owner(p)!=self or claim(p)==0xff); or claim(p)==0xff; or
   any of self's claimed records j<0x32 links to p (link-1==p among its 8 links).
   Returns 0 if owner(p)==self with a live claim, or after the full j scan.

   PARKED at 314/297 (structure correct, tails duplicate correctly). PROVEN HERE:
   single-exit + goto form is REQUIRED — multi-return C with per-return restores
   splits `save` across ESI/EDI (+push edi). Residual walls: (1) entry scheduler
   batches cl/g_539a/si loads and loads g_539a into AH instead of the target's
   direct `cmp byte [mem],0`; (2) the (unsigned short) cast widens via half-clear
   `xor dh,dh` where target re-clears full EDX (dropping the cast promotes save
   to a 32-bit copy instead — worse); (3) -oneatx hoists the loop's -1 into CH
   (`add al,ch`) where target has `dec al`; named-local v didn't break the hoist.
   cpermute 4000 variants: no match. */
extern unsigned char g_539a;
extern unsigned short g_10b16;
extern unsigned char g_539c[];
extern unsigned char g_b069[];

char FUN_000264a8(unsigned char p)
{
    unsigned char j, k, v;
    unsigned short save;
    char ret;

    save = g_10b16;
    if (g_539a != 0) {
        ret = 1;
        goto done;
    }
    if ((unsigned short)g_539c[p * 10 + 2] == save) {
        if (*(unsigned short *)(g_539c + p * 10) != 0xff) {
            ret = 0;
            goto done;
        }
    }
    if (g_b069[p * 19 + 8] == 1) {
        if ((unsigned short)g_539c[p * 10 + 2] != save
            || *(unsigned short *)(g_539c + p * 10) == 0xff) {
            ret = 1;
            goto done;
        }
    }
    if (*(unsigned short *)(g_539c + p * 10) == 0xff) {
        ret = 1;
        goto done;
    }
    for (j = 0; j < 0x32; j++) {
        if ((unsigned short)g_539c[j * 10 + 2] != save)
            continue;
        if (*(unsigned short *)(g_539c + j * 10) == 0xff)
            continue;
        for (k = 0; k != 8; k++) {
            v = g_b069[j * 19 + k] - 1;
            if (v == p) {
                ret = 1;
                goto done;
            }
        }
    }
    ret = 0;
done:
    g_10b16 = save;
    return ret;
}
