/* frameless @ 0x22708: zero the region from g_108 up to g_1c632. Calls
   FUN_0004d199(g_108, 0, g_1c632 - g_108); the size is the runtime difference of the
   two symbol addresses (end - start), not a folded constant. */
extern unsigned char g_108[];
extern unsigned char g_1c632[];
extern void FUN_0004d199(void *dst, int val, int n);
void FUN_00022708(void)
{
    FUN_0004d199(g_108, 0, g_1c632 - g_108);
}
