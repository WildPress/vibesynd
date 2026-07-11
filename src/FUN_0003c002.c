/* framed indirect forwarder @ 0x3c002: push [ebp+10]; push [ebp+c]; push [ebp+8]; call [c2e8] */
extern void (*g_c2e8)(int a, int b, int c);
void FUN_0003c002(int a, int b, int c)
{
    g_c2e8(a, b, c);
}
