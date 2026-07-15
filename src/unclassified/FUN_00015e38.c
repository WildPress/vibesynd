/* frameless @ 0x15e38: masked blit of a param_5 x param_6 cell into the g_screen_buf buffer.
   mask = param_1*param_6*param_5 + param_7; fill = g_blip_colours[param_2]; dest origin =
   g_screen_buf + param_3 + param_4*0x140. For each of param_6 rows x param_5 cols: write 0
   where the mask byte is 0, else the fill byte. Rows step 0x140 - param_5.

   PARKED near-miss (NOT matched; logic correct). Scheduling wall: with 7 params under
   register pressure, the target loads param_2 into CL first and schedules the two IMULs,
   the g_screen_buf load, and the fill deref in an order no source spelling reproduces (we load
   the multiply operands first). Statement reorder + multiply regrouping + 5000 cpermute
   variants plateau at 122/162. Not source-reachable. */
extern unsigned char *g_screen_buf;
extern unsigned char g_blip_colours[];
void FUN_00015e38(unsigned char param_1, unsigned char param_2, unsigned short param_3,
                  unsigned short param_4, unsigned char param_5, unsigned char param_6, int param_7)
{
    unsigned char fill = g_blip_colours[param_2];
    unsigned char *mask = (unsigned char *)(param_1 * (param_6 * param_5) + param_7);
    unsigned char *dest = g_screen_buf + param_3 + param_4 * 0x140;
    unsigned char local_10 = 0;
    if (param_6 != 0) {
        do {
            unsigned char col = 0;
            if (param_5 != 0) {
                do {
                    if (*mask == 0)
                        *dest = 0;
                    else
                        *dest = fill;
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
