/* framed forwarder @ 0x3e471: push [c2f4]; lea eax,[ebp+10]; push eax; push [ebp+c]; push [ebp+8]; call 0x3cc74 */
extern int g_c2f4;
extern void FUN_0003cc74(int a, int b, int *pc, int g);
void FUN_0003e471(int a, int b, int c)
{
    FUN_0003cc74(a, b, &c, g_c2f4);
}
