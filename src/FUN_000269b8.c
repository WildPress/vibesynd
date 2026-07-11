/* @ 0x000269b8 (22B): forward arg to a callee, then clear a field.
 *   mov edx,[esp+4]; push edx; call 0x26da8; add esp,4;
 *   mov eax,[esp+4]; mov byte[eax+0x18],0; ret
 */
extern void FUN_00026da8(void *);
void FUN_000269b8(char *p)
{
    FUN_00026da8(p);
    p[0x18] = 0;
}
