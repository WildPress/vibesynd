/* framed forwarder @ 0x3e48e: push [c2f4]; push [ebp+10]; push [ebp+c]; push [ebp+8]; call 0x3e6ab */
extern int g_c2f4;
extern void spawn_search_path(int a, int b, int c, int g);
void exec_with_env(int a, int b, int c)
{
    spawn_search_path(a, b, c, g_c2f4);
}
