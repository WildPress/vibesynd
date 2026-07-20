/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): pure CODEGEN TIE. Full hex decode
 * confirms identical record geometry (10-byte recs: claim word +0, owner byte +2; 19-byte
 * rows: 8 link bytes +0, flag +8), the same block order and outcomes (g_539a->1;
 * owner==save&&claim!=0xff->0; flag==1&&(owner!=save||claim==0xff)->1; claim==0xff->1;
 * j<0x32 x k!=8 scan for link-1==p->1 else 0), the g_cur_player save/restore, and the
 * shared mov al,1 tail. Divergences are register-idiom ties only: entry order of loading
 * p vs save, half-clear `xor dh,dh`/`xor dh,bh` vs full `xor edx,edx` for the owner-byte
 * widen, and a block-2 `mov edx,ebx` before the widen. Same dx value, same behaviour.
 *
 * frameless @ 0x264a8: slot-claim eligibility test for record p (char return).
   Tables: 10-byte records at 0x539c (word at +0 = claim word, byte at +2 = owner)
   and 19-byte rows at 0xb069 (8 link bytes at +0, flag at +8) — same pair 0x164c8
   walks. save = g_cur_player (self), restored at the single exit (Watcom tail-
   duplicates the store+ret into the branches). Returns 1 if: g_539a set; or
   row-flag[p]==1 and (owner(p)!=self or claim(p)==0xff); or claim(p)==0xff; or
   any of self's claimed records j<0x32 links to p (link-1==p among its 8 links).
   Returns 0 if owner(p)==self with a live claim, or after the full j scan.

   EDIT-DIST 51 / 297 bytes (down from 73). Single-exit + goto form is REQUIRED:
   multi-return C with per-return restores splits `save` across ESI/EDI (+push
   edi). Two of the three old walls were cracked here:
   - g_539a and g_cur_player marked `volatile`: g_539a now compiles to the
     target's direct `cmp byte [539a],0` (was a hoisted `mov ah,[539a]; test
     ah,ah`), and it also removed a redundant `mov edx,ebx` and tightened the
     entry (73 -> 66).
   - The two `ret=1` tails (row-flag block and the standalone claim==0xff block)
     now SHARE one copy: block2's success does `goto ret1` into the middle of
     block3's if-body, reproducing the target's forward-jump to a single
     `mov al,1; store; ret`. This restructuring also made the loop's `-1`
     compile to `dec al` (the old -oneatx CH-hoist vanished) (66 -> 51).
   Residual walls (same-length register-idiom ties): (1) entry scheduler loads
   the param p (cl) before save (si) where the target loads si first; (2) the
   (unsigned short) owner-byte widen uses half-clear `xor dh,dh` (and, in
   block2, an extra `mov edx,ebx; xor dh,bh`) where the target re-clears full
   EDX — dropping the cast promotes save to a 32-bit copy instead, worse. */
extern volatile unsigned char g_539a;
extern volatile unsigned short g_cur_player;
extern unsigned char g_syndicate_recs[];
extern unsigned char g_b069[];

char slot_claim_test(unsigned char p)
{
    unsigned char j, k, v;
    unsigned short save;
    char ret;

    save = g_cur_player;
    if (g_539a != 0) {
        ret = 1;
        goto done;
    }
    if ((unsigned short)g_syndicate_recs[p * 10 + 2] == save) {
        if (*(unsigned short *)(g_syndicate_recs + p * 10) != 0xff) {
            ret = 0;
            goto done;
        }
    }
    if (g_b069[p * 19 + 8] == 1) {
        if ((unsigned short)g_syndicate_recs[p * 10 + 2] != save
            || *(unsigned short *)(g_syndicate_recs + p * 10) == 0xff) {
            goto ret1;
        }
    }
    if (*(unsigned short *)(g_syndicate_recs + p * 10) == 0xff) {
ret1:
        ret = 1;
        goto done;
    }
    for (j = 0; j < 0x32; j++) {
        if ((unsigned short)g_syndicate_recs[j * 10 + 2] != save)
            continue;
        if (*(unsigned short *)(g_syndicate_recs + j * 10) == 0xff)
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
    g_cur_player = save;
    return ret;
}
