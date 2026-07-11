/* framed forwarder @ 0x3cfce: lea eax,[ebp+10]; push eax; push [ebp+c]; push [ebp+8]; call 0x3e48e */
extern void FUN_0003e48e(int a, int b, int *pc);
void FUN_0003cfce(int a, int b, int c)
{
    FUN_0003e48e(a, b, &c);
}
