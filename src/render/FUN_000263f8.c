/* frameless @ 0x263f8: masked blit of a 0x40 x 0x2c cell into a destination. mask =
   g_df18 + param_1*0xb00; for each of 0x2c rows x 0x40 cols, if the mask byte is set
   write param_2 to the destination. The destination origin is g_b060[param_1]/2 +
   g_b062[param_1]*0x140/2 + param_3 (records of 0x13 bytes); rows step 0x140 (0x40
   written inner + 0x100 extra). */
extern unsigned char *g_df18;
extern short g_b060[];
extern short g_b062[];
void FUN_000263f8(unsigned char param_1, unsigned char param_2, int param_3)
{
    unsigned char *mask = g_df18 + param_1 * 0xb00;
    int idx = param_1 * 0x13;
    unsigned char *dest = (unsigned char *)(
        *(unsigned short *)((char *)g_b060 + idx) / 2 +
        *(unsigned short *)((char *)g_b062 + idx) * 0x140 / 2 +
        param_3);
    unsigned char outer = 0;
    do {
        unsigned char inner = 0;
        do {
            if (*mask != 0)
                *dest = param_2;
            mask++;
            inner++;
            dest++;
        } while (inner != 0x40);
        dest += 0x100;
        outer++;
    } while (outer != 0x2c);
}
