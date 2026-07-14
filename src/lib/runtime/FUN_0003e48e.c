/* framed forwarder @ 0x3e48e: push [c2f4]; push [ebp+10]; push [ebp+c]; push [ebp+8]; call 0x3e6ab */
extern int g_c2f4;
extern void FUN_0003e6ab(int a, int b, int c, int g);
void FUN_0003e48e(int a, int b, int c)
{
    FUN_0003e6ab(a, b, c, g_c2f4);
}
