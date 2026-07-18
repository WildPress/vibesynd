/* frameless guard-then-init/allocate @ 0x25238 (stack-calling, -4s).
   If either flag set: allocate handle (0x39625), store to g_defc; unless -1,
   size it (0x39846) and register it (0x39747). Else: save old timer vector
   (d_getvec 8 -> g_df08 far ptr), program the PIT (out 0x3b22d), and install
   the new far ISR (d_setvec 8, cs:FUN_00017a90).

   NEAR-MISS 143/146 (default -4s -oneatx; every -o* recipe gives the same 143).
   The entire instruction stream is byte-IDENTICAL (masked): both guard compares
   and the || short-circuit (jnz/jz), the whole first path incl. the g_defc reload
   (8b15fcde0000 52) for the last call, the far d_getvec->g_df08 store order
   (6689150cdf0000 / a308df0000), all three out() calls, and the push-cs far ISR
   idiom (0e 68<17a90> 6a08). The far-ptr casts of code addrs (near (void*) for
   0x17a70, far (void __far*) for cs:0x17a90) reproduce exactly.
   SOLE diff: the target has a DEAD `push ebx` (53) at entry + `pop ebx` (5b) at
   BOTH rets (3 bytes) though EBX is never read/written in the body. Our 9.5b never
   leaves EBX dead: forcing a local into EBX makes it USE ebx (adds mov ebx,eax +
   push ebx for the last call, and no g_defc reload) -> 142, wrong. The target
   reserves EBX yet still reloads g_defc, i.e. EBX holds nothing -- the cluster
   dead-callee-save wall (identical to parked 0x36168, 0x39188). Not source-
   reachable at this compiler; block/reg permuter may close it. */
extern unsigned char g_sound_enabled, g_music_enabled;
extern int g_defc;

extern void FUN_00017a70(void);
extern void FUN_00017a90(void);

extern int  FUN_00039625(void *p);
extern void timer_rate_critsec(int a, int b);
extern void FUN_00039747(int a);

extern void __far *d_getvec(unsigned n);
extern void outp(int a, int b);
extern void d_setvec(int a, void __far *p);

extern void __far *g_df08;   /* off @0xdf08, seg @0xdf0c */

void guarded_init_alloc(void)
{
    if (g_sound_enabled != 0 || g_music_enabled != 0) {
        g_defc = FUN_00039625((void *)FUN_00017a70);
        if (g_defc != -1) {
            timer_rate_critsec(g_defc, 0x48);
            FUN_00039747(g_defc);
            return;
        }
    } else {
        g_df08 = d_getvec(8);
        outp(0x43, 0x36);
        outp(0x40, 0);
        outp(0x40, 0x40);
        d_setvec(8, (void __far *)FUN_00017a90);
    }
}
