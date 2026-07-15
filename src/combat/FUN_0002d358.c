/* frameless @ 0x2d358: combat/aim update, cousin of 0x2d6c8/0x2d738. Set
   obj[0x54] = FUN_0002d8c8(obj, obj[0x55]) (speed from a table), then aim toward the
   target: r = FUN_00026bc8(obj[0x54], obj[0x2e]-g_aim_x, obj[0x30]-g_aim_y) where the
   coord diffs are signed shorts. FUN_00026bc8 returns short (the copy-to-EDX + TEST AX
   codegen only reproduces with a short return). If r >= 0 store the facing obj[0x1a]=(char)r,
   otherwise call FUN_0002d998(obj). */
extern short g_aim_x;
extern short g_aim_y;
extern int FUN_0002d8c8(unsigned char *obj, short a);
extern short FUN_00026bc8(unsigned short spd, int dx, int dy);
extern void FUN_0002d998(unsigned char *obj);
void FUN_0002d358(unsigned char *obj)
{
    short r;
    obj[0x54] = FUN_0002d8c8(obj, obj[0x55]);
    r = FUN_00026bc8(obj[0x54],
                     (short)(*(short *)(obj + 0x2e) - g_aim_x),
                     (short)(*(short *)(obj + 0x30) - g_aim_y));
    if (r >= 0) {
        obj[0x1a] = (char)r;
        return;
    }
    FUN_0002d998(obj);
}
