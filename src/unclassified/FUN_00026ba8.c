/* short-arg forwarder @ 0x00026ba8 (21B):
 *   movsx eax,[esp+8]; push eax; movsx eax,[esp+8]; push eax; call 0x4d221; add esp,8; ret
 * Forwards two sign-extended short params to a 2-int function. Stack calling. */
extern int FUN_0004d221(int, int);
int FUN_00026ba8(short a, short b)
{
    return FUN_0004d221(a, b);
}
