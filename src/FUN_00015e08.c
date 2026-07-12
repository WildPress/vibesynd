/* 0x15e08 -- 5-arg constant forwarder into FUN_00015ee8.
 * xor eax,eax; mov al,[esp+4] = unsigned char param widened.
 * Push order: a, 0x96, 0x264, 0x16, 0x118 -> args (0x118,0x16,0x264,0x96,a).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern void FUN_00015ee8(int a, int b, int c, int d, int e);

void FUN_00015e08(unsigned char a)
{
    FUN_00015ee8(0x118, 0x16, 0x264, 0x96, a);
}
