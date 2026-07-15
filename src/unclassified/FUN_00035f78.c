/* frameless @ 0x35f78: guarded dispatch. If g_sound_enabled && g_537f, index the
   0x20-stride record table g_objective_slots by param_1, then call FUN_00039ae7 passing
   g_11dec, a far pointer built from *(int*)(rec+0x12) with the DS segment
   (mov dx,ds / and edx,0xffff idiom, cf. sibling 0x35e68), *(int*)(rec+0xe)
   and -1; then FUN_00039b05(g_11dec). Stack-calling (-4s).

   NEAR-MISS / WALL (§3 coupled register-role + address-fold). 90/90 bytes;
   byte-identical through the prologue (push ebx/esi = 5356), both guard
   compares, the far-ptr DS idiom (8cda 81e2ffff0000), the g_11dec ecx/esi
   double-load, both calls and the tail (5e5bc3). ONLY the addressing region
   differs (first diff 0x24):
     target UN-FOLDS: add eax,edx (01d0); mov ebx,[eax+0xe] (8b580e);
                      ... mov eax,[eax+0x12] (8b4012)  -- frees EDX for mov dx,ds
     ours    FOLDS  : mov ebx,[edx+eax+0xe] (8b5c020e);
                      mov eax,[edx+eax+0x12] (8b440212) via SIB, hoisting +0x12.
   Forcing the add (int locals / two-step p) DOES un-fold but the allocator then
   drops push esi (88B) or reshuffles the guard/param loads -- our Watcom 9.5b
   cannot emit {5356 prologue + ESI role + un-folded base} together, which is
   exactly the target's colouring. Not source-reachable at this recipe. */
extern unsigned char *g_objective_slots;
extern int g_11dec;
extern unsigned char g_sound_enabled, g_537f;
extern void FUN_00039ae7(int a, void __far *p, int b, int c);
extern void FUN_00039b05(int a);

void FUN_00035f78(unsigned char param_1)
{
    unsigned char *p;

    if (g_sound_enabled != 0 && g_537f != 0) {
        p = g_objective_slots + param_1 * 0x20;
        FUN_00039ae7(g_11dec, (void __far *)(*(int *)(p + 0x12)),
                     *(int *)(p + 0xe), -1);
        FUN_00039b05(g_11dec);
    }
}
