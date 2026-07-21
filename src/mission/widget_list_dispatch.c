/* inline jump-table switch dispatcher @ 0x2bca8 (target 550B; ours 590B, EDIT-DIST=148).
 * NOTE ~34B of the 40B length gap and much of the distance is our inline jump-table
 * emitted before the body (measurement artifact); the real code diff is tiny (walls below).
 * command-list interpreter.
 *
 * Signature: 4 stack params (Ghidra's 1-param __cdecl is phantom):
 *   FUN(unsigned char *p /[ESP+0x10]->EBX/, int sel /[ESP+0x14]->ESI/,
 *       int setX /[ESP+0x18]/, int setY /[ESP+0x1c]/)
 * Walks 0x12-byte records; word[0]==-0x63 terminates. 5-way switch on word[0].
 *
 * Recipe: -4s -oneatx -zp8 -s -zq  (the `-ox` sub-flag is REQUIRED for the box-test
 * operand orientation; -oneat/-oneata/-oneati/-oneatl all get the box tests wrong).
 *
 * NEAR-MISS (byte-faithful structure; residual diffs are -ox scheduler / phase-order
 * walls, playbook Section 3 — every source spelling + recipe I tried shuffles them):
 *   1. `g_sel_cursor = 1` (the FIRST flag=1 store in each of the two select blocks) emits
 *      `mov reg,1; mov [g_sel_cursor],reg` where the target uses the immediate `mov word
 *      [g_sel_cursor],1`. This is a pure -ox delay-slot-fill; the SECOND flag=1 (g_e114) is
 *      immediate in both. Adds ~+3B each => length +6..8 => most other "diff regions"
 *      are just cascaded jump-displacement fallout.
 *   2. Entry: the duplicated early-return tail folds `return result` (result==0) to
 *      `xor eax,eax` where the target keeps `mov eax,edi`; and `xor edi,edi` schedules
 *      after the guard `movsx` instead of before. Net 0 length, phase-order artifact.
 *   3. FIXED (150->148): case-2's OR test now spelled `(setX | setY)` (was `(setY|setX)`),
 *      which makes Watcom load setX before setY to match the target's operand order.
 *      Remaining: the loop-tail does `movsx eax,[ebx+0x12]; add ebx` vs target
 *      `add ebx; movsx eax,[ebx]` -- a -ox induction/schedule fold, not source-reachable
 *      (volatile read on the while-condition was byte-inert; fuzzer-reachable at best).
 * Levers already applied: volatile g_cursor_x/g_cursor_y (target RE-READS them each compare),
 * unsigned char setX/setY (avoids the EBP hoist), unsigned short return (duplicated
 * epilogue vs shared), swapped box-test operand orders (acc = right operand).
 *
 * cont.22 RETRY (parked again 558/550 code bytes; 5 compiles, all reverted):
 *   - volatile-alias extern (the 0x2bee8 case-3 lever) does NOT transfer: alias on
 *     just the first `=1` store is byte-INERT in case 0 and in case 2 it flips the
 *     case-2 x-block regs (schar lands EDX not EAX) without fixing the store. All-four
 *     stores through the alias in case 2 produce the bee8-case-3 shape (mem-cmp
 *     `cmp byte[esp+18],0` + IMMEDIATE `=0` stores) -- but THIS target keeps
 *     register-loaded setX/setY tests and reg-form `=0`, so bee8's target genuinely
 *     differed there; our first-`=1` reg-form is the same wall as bee8's CASE 1.
 *   - store-order swap `{ g_sel_cursor = 1; g_target_pending = 0; }`: scheduler reorders back, still
 *     `mov ecx,1` reg-form. The imm-vs-reg pick is register-availability driven (2nd
 *     `=1` is imm only because ECX is wanted by the following `mov cx,[ebx]`).
 *   - `goto Lret;` to a shared final return: Watcom CROSS-JUMPS (jz rel32) instead of
 *     duplicating the epilogue -- loses the inline dup, -2B, worse.
 *   - plain `while` loop: FIXES the entry `xor edi,edi` placement (lands before the
 *     guard movsx, proving that half of wall 2 is block-boundary-driven) but -oneatx
 *     then emits jump-to-test layout + per-case duplicated `add ebx,0x12; jmp top`
 *     advances (571B) -- entry schedule and loop layout CO-VARY, same wall class as
 *     the playbook 2 structured-loop-var entry. Target = do-while shared bottom test.
 */
extern volatile unsigned short g_cursor_x, g_cursor_y;   /* cursor point (x, y) */
extern unsigned short g_e114, g_sel_cursor;     /* selection cursor state words */
extern unsigned char  g_10b3e, g_target_pending;   /* selection cursor flags */
extern short FUN_00029988(unsigned char *p);

unsigned short widget_list_dispatch(unsigned char *p, int sel, unsigned char setX, unsigned char setY)
{
    unsigned short result = 0;

    if (*(short *)p == -0x63)
        return result;
    do {
        switch (*(unsigned short *)p) {
        default:
            goto Lnext;

        case 0:                                 /* block 0x2bcdd */
            if ((setX | setY) == 0) goto A_47;
            {
                int x = *(signed char *)(p + 8) + (unsigned short)*(unsigned short *)(p + 2);
                if (g_cursor_x < x) goto A_47;
                x += p[6];
                if (g_cursor_x >= x) goto A_47;
            }
            {
                int y = (unsigned short)*(unsigned short *)(p + 4) + *(signed char *)(p + 9);
                if (g_cursor_y < y) goto A_47;
                y += p[7];
                if (g_cursor_y < y) goto A_58;
            }
        A_47:
            if ((unsigned short)sel != *(signed char *)(p + 0xb)) goto Lnext;
        A_58:
            g_sel_cursor = 0;
            if (setX) { g_target_pending = 0; g_sel_cursor = 1; }
            g_e114 = 0;
            if (setY) { g_10b3e = 0; g_e114 = 1; }
            {
                short t = *(short *)p;
                p[0xa] = 2;
                *(short *)p = t + 2;
            }
            goto Lnext;

        case 1:                                 /* block 0x2bdad */
            if (FUN_00029988(p))
                (*(short *)p)++;
            goto Lpa;

        case 2:                                 /* block 0x2bdc7 */
            if (p[0xa] != 0) {
                result = *(signed char *)(p + 0xb);
                goto Lnext;
            }
            if ((setX | setY) == 0) goto C_93;
            {
                int x = (unsigned short)*(unsigned short *)(p + 2) + *(signed char *)(p + 8);
                if (g_cursor_x < x) goto C_93;
                x += p[6];
                if (g_cursor_x >= x) goto C_93;
            }
            {
                int y = *(signed char *)(p + 9) + (unsigned short)*(unsigned short *)(p + 4);
                if (g_cursor_y < y) goto C_93;
                y += p[7];
                if (g_cursor_y >= y) goto C_93;
            }
            g_sel_cursor = 0;
            if (setX) { g_target_pending = 0; g_sel_cursor = 1; }
            g_e114 = 0;
            if (setY) { g_10b3e = 0; g_e114 = 1; }
            goto Lpa;
        C_93:
            if ((unsigned short)sel == *(signed char *)(p + 0xb)) goto Lnext;
            goto Lp0_1;

        case 4:                                 /* block 0x2bea2 */
            if (FUN_00029988(p) == 0) goto Lpa;
        Lp0_1:
            *(short *)p = 1;
        Lpa:
            p[0xa] = 2;
        }
    Lnext:
        p += 0x12;
    } while (*(short *)p != -0x63);

    return result;
}
