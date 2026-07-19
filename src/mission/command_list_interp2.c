/* inline jump-table switch dispatcher @ 0x2c218 (592B) -- command-list interpreter.
 *   Sibling of widget_list_dispatch (same shape; register roles swapped: sel->EDI,
 *   result->ESI). Extra `if(result==0)` clause in case 2 and a post-switch
 *   g_df75[(signed char)p[0xb]] = 0 tail (runs when p[0xa] != 0).
 *
 * Signature: 4 stack params (Ghidra's 1-param __cdecl is phantom):
 *   FUN(unsigned char *p /[ESP+0x10]->EBX/, int sel /[ESP+0x14]->EDI/,
 *       int setX /[ESP+0x18]/, int setY /[ESP+0x1c]/)
 * Walks 0x12-byte records; word[0]==-0x63 terminates. 5-way switch on word[0].
 *
 * Recipe: -4s -oneatx -zp8 -s -zq  (see sibling widget_list_dispatch for the -ox rationale).
 *
 * NEAR-MISS (byte-faithful structure; same -ox scheduler walls as the sibling PLUS a
 * register-role wall, playbook Section 3):
 *   1. Same `mov reg,1; mov [g_sel_cursor],reg` vs immediate on the first flag=1 store, and
 *      the same entry early-return fold — identical to widget_list_dispatch.
 *   2. REGISTER-ROLE: the target colours result->ESI and sel->EDI (because result is
 *      read in case 2, giving it more uses); our compile colours sel->ESI, result->EDI
 *      (like the sibling), so every `mov dx,di`(sel) and `mov eax,esi`(result) has the
 *      other register. No declaration/first-use order I found flips the allocator.
 *
 * cont.22 RETRY (parked again 600/592 code bytes; 4 compiles, all reverted) -- three
 * more role-flip levers failed, plus see widget_list_dispatch's header for why the 0x2bee8
 * volatile-alias lever does not transfer to this family's `=1` stores:
 *   - `register unsigned short result`: byte-inert, no flip.
 *   - ternary double-read `result = result ? result : (unsigned short)*(signed char *)
 *     (p+0xb);`: coalesces to the IDENTICAL test/jnz/movsx bytes -- extra source-level
 *     use does not raise the allocator weight. Use-count weighting is not the tiebreak.
 *   - `goto Lret;` shared-return (to un-fold the early `mov eax,edi` use): no flip,
 *     entry becomes jz rel32 (cross-jump, no dup epilogue), AND it perturbs the case-0
 *     x-block registers (schar->EAX flips to EDX). Reverted.
 */
extern volatile unsigned short g_cursor_x, g_cursor_y;   /* cursor point (x, y) */
extern unsigned short g_e114, g_sel_cursor;     /* selection cursor state words */
extern unsigned char  g_10b3e, g_10b3f;   /* selection cursor flags */
extern unsigned char  g_df75[];           /* record-indexed flag table (tail clears) */
extern short FUN_00029988(unsigned char *p);

unsigned short command_list_interp2(unsigned char *p, int sel, unsigned char setX, unsigned char setY)
{
    unsigned short result = 0;

    if (*(short *)p == -0x63)
        return result;
    do {
        switch (*(unsigned short *)p) {
        default:
            goto Lcheck;

        case 0:                                 /* block 0x2c24d */
            if ((setX | setY) == 0) goto A_47;
            {
                int x = *(signed char *)(p + 8) + (unsigned short)*(unsigned short *)(p + 2);
                if (g_cursor_x < x) goto A_47;
                x += p[6];
                if (g_cursor_x >= x) goto A_47;
            }
            {
                int y = *(signed char *)(p + 9) + (unsigned short)*(unsigned short *)(p + 4);
                if (g_cursor_y < y) goto A_47;
                y += p[7];
                if (g_cursor_y < y) goto A_58;
            }
        A_47:
            if ((unsigned short)sel != *(signed char *)(p + 0xb)) goto Lcheck;
        A_58:
            g_sel_cursor = 0;
            if (setX) { g_10b3f = 0; g_sel_cursor = 1; }
            g_e114 = 0;
            if (setY) { g_10b3e = 0; g_e114 = 1; }
            {
                short t = *(short *)p;
                p[0xa] = 2;
                *(short *)p = t + 2;
            }
            goto Lwrite;

        case 1:                                 /* block 0x2c31d */
            if (FUN_00029988(p))
                (*(short *)p)++;
            p[0xa] = 2;
            goto Lwrite;

        case 2:                                 /* block 0x2c337 */
            if (p[0xa] != 0) {
                if (result == 0)
                    result = *(signed char *)(p + 0xb);
                goto Lcheck;
            }
            if ((setY | setX) == 0) goto C_93;
            {
                int x = (unsigned short)*(unsigned short *)(p + 2) + *(signed char *)(p + 8);
                if (g_cursor_x < x) goto C_93;
                x += p[6];
                if (g_cursor_x >= x) goto C_93;
            }
            {
                int y = (unsigned short)*(unsigned short *)(p + 4) + *(signed char *)(p + 9);
                if (g_cursor_y < y) goto C_93;
                y += p[7];
                if (g_cursor_y >= y) goto C_93;
            }
            g_sel_cursor = 0;
            if (setX) { g_10b3f = 0; g_sel_cursor = 1; }
            g_e114 = 0;
            if (setY) { g_10b3e = 0; g_e114 = 1; }
            p[0xa] = 2;
            goto Lwrite;
        C_93:
            if ((unsigned short)sel == *(signed char *)(p + 0xb)) goto Lcheck;
            *(short *)p = 1;
            p[0xa] = 2;
            goto Lwrite;

        case 4:                                 /* block 0x2c428 */
            if (FUN_00029988(p))
                *(short *)p = 1;
            p[0xa] = 2;
            goto Lwrite;
        }
    Lcheck:                                     /* 0x2c441 */
        if (p[0xa] == 0) goto Linc;
    Lwrite:                                     /* 0x2c447 */
        g_df75[*(signed char *)(p + 0xb)] = 0;
    Linc:                                       /* 0x2c453 */
        p += 0x12;
    } while (*(short *)p != -0x63);

    return result;
}
