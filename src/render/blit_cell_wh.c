/* frameless @ 0x15e38: masked blit of a param_5 x param_6 cell into the g_screen_buf buffer.
   mask = param_1*param_6*param_5 + param_7; fill = g_blip_colours[param_2]; dest origin =
   g_screen_buf + param_3 + param_4*0x140. For each of param_6 rows x param_5 cols: write 0
   where the mask byte is 0, else the fill byte. Rows step 0x140 - param_5.

   NEAR-MISS (logic correct). EDIT-DIST 34 (was 59). The whole inner copy loop and the
   dest-pointer setup now match byte-for-byte. Fixes: (1) the inner test writes fill on
   the mask!=0 path -- `if (*mask != 0) *dest = fill; else *dest = 0;` -- giving the
   target's `je` polarity with the just-loaded zero byte reused for the clear.
   (2) grouping the dest offset `g_screen_buf + (param_4*0x140 + param_3)` makes the
   compiler build the offset first (param_4*0x140 in ESI, +param_3, +g_screen_buf) exactly
   as the target does, instead of splitting g_screen_buf+param_3 into EDI early.
   (3) inner multiply spelled `param_5 * param_6` matches the target's imul operand order.
   Residual (~34): register-role / scheduling ties in the mask setup only. The target
   keeps the mask accumulator in EDX throughout so param_7 folds as `add edx,[esp+0x2c]`,
   while ours builds the product in EAX then loads param_7 separately; and the target
   loads param_2 into CL first (masking later with `and ecx,0xff`) where ours zeroes ECX
   then loads. These are the eax<->edx accumulator and param_2 load-order ties; no source
   spelling reproduces them.

   FLAG improvement (2026-07-20 sweep): dropping the relax-alias letter 'a' from the
   bundle -- recipe -4s -onetx -zp8 -s -zq -- lowers the residual 34->27 with no length
   change (the param_7 fold half comes back). Still NOT a match: the param_2 load-order /
   accumulator half remains a Watcom-minor-version codegen tie no flag or source reaches.
   Recorded as this function's parked recipe (closest known near-miss). */
extern unsigned char *g_screen_buf;
extern unsigned char g_blip_colours[];
void blit_cell_wh(unsigned char param_1, unsigned char param_2, unsigned short param_3,
                  unsigned short param_4, unsigned char param_5, unsigned char param_6, int param_7)
{
    unsigned char fill = g_blip_colours[param_2];
    unsigned char *mask = (unsigned char *)(param_1 * (param_5 * param_6) + param_7);
    unsigned char *dest = g_screen_buf + (param_4 * 0x140 + param_3);
    unsigned char local_10 = 0;
    if (param_6 != 0) {
        do {
            unsigned char col = 0;
            if (param_5 != 0) {
                do {
                    if (*mask != 0)
                        *dest = fill;
                    else
                        *dest = 0;
                    mask++;
                    col++;
                    dest++;
                } while (col != param_5);
            }
            local_10++;
            dest += 0x140 - param_5;
        } while (local_10 != param_6);
    }
}
