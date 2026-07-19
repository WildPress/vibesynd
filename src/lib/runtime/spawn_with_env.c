/* framed forwarder @ 0x3e471: push [c2f4]; lea eax,[ebp+10]; push eax; push [ebp+c]; push [ebp+8]; call 0x3cc74 */
extern int g_c2f4;
extern void spawn_exec_core(int a, int b, int *pc, int g);
void spawn_with_env(int a, int b, int c)
{
    spawn_exec_core(a, b, &c, g_c2f4);
}
