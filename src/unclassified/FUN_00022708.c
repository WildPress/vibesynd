/* frameless @ 0x22708: zero the region from g_rng_seed up to g_1c632. Calls
   FUN_0004d199(g_rng_seed, 0, g_1c632 - g_rng_seed); the size is the runtime difference of the
   two symbol addresses (end - start), not a folded constant. */
extern unsigned char g_rng_seed[];
extern unsigned char g_1c632[];
extern void FUN_0004d199(void *dst, int val, int n);
void FUN_00022708(void)
{
    FUN_0004d199(g_rng_seed, 0, g_1c632 - g_rng_seed);
}
