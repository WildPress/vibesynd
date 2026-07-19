/* @ 0x000269b8 (22B): forward arg to a callee, then clear a field.
 *   mov edx,[esp+4]; push edx; call 0x26da8; add esp,4;
 *   mov eax,[esp+4]; mov byte[eax+0x18],0; ret
 */
extern void grid_unlink_object(void *);
void FUN_000269b8(char *p)
{
    grid_unlink_object(p);
    p[0x18] = 0;
}
