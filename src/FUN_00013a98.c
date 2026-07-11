/* frameless @ 0x13a98: if arg2->[0xb] bit 0 is set, forward to FUN_13ac8 with
   reordered args and return 0; otherwise return 1. */
extern void FUN_00013ac8(int a, int b, int c);
char FUN_00013a98(int arg1, unsigned char *arg2, int arg3, unsigned char arg4)
{
    char result = 1;
    if (arg2[0xb] & 1) {
        result = 0;
        FUN_00013ac8(arg3, arg1, arg4);
    }
    return result;
}
