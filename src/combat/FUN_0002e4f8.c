/* frameless @ 0x2e4f8: search up to 4 directions for a valid projectile step.
   local_10 = obj[0x1a] + param_2; each iteration test FUN_0002d5b8 at the position
   obj[4]/obj[6] offset by the signed direction tables g_dir_dx (x) / g_dir_dy (y)
   scaled by obj[0x54] (>>8), with obj[8] as the third arg. Stop when the (short)
   result is nonzero; otherwise step local_10 -= param_2. Store final into obj[0x1a]. */
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int FUN_0002d5b8(int x, int y, int z, unsigned char *obj);
void FUN_0002e4f8(unsigned char *obj, unsigned char param_2)
{
    unsigned char local_10;
    short i;
    local_10 = obj[0x1a] + param_2;
    for (i = 0; i < 4; i++) {
        if ((short)FUN_0002d5b8(
                (short)(*(short *)(obj + 4) + (g_dir_dx[local_10] * obj[0x54] >> 8)),
                (short)((g_dir_dy[local_10] * obj[0x54] >> 8) + *(short *)(obj + 6)),
                *(short *)(obj + 8),
                obj) != 0)
            break;
        local_10 -= param_2;
    }
    obj[0x1a] = local_10;
}
