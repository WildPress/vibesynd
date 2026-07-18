/* framed 2-arg forwarder @ 0x3aee6: push [ebp+c]; push [ebp+8]; call 0x3aef9; add esp,8 */
extern void stricmp(int a, int b);
void stricmp_call(int a, int b)
{
    stricmp(a, b);
}
