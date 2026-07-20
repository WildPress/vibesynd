/* frameless guard-then-init/allocate @ 0x25238 (stack-calling, -4s), 146 bytes.
   If either sound flag is set: allocate a handle (alloc_seq_slot, 0x39625) and
   store it to g_defc. Unless the handle is -1, size it (timer_rate_critsec,
   0x39846) and register it (start_seq, 0x39747). Otherwise: save the old timer
   vector (d_getvec 8 -> g_df08 far ptr), reprogram the PIT (three outp calls),
   and install the new far ISR (d_setvec 8, cs:FUN_00017a90).

   MATCHED (byte-exact, relocation-aware) at recipe -4s -oneatx -zp8 -s -zq via
   the callee-modify lever.

   The whole instruction stream was already byte-identical bar one thing: the
   target saves EBX once around the entire body. It pushes EBX at entry (before
   the sound/music branch) and pops it at BOTH rets, yet never reads or writes
   EBX in between. It is a dead, hoisted callee-save. Our compiler will not emit
   it from plain C because nothing puts a live value in EBX, and forcing a local
   into EBX makes the compiler USE ebx (extra mov/push, no g_defc reload) which
   is the wrong shape.

   The lever: declare a called routine as clobbering EBX. Watcom then reserves
   and saves EBX for this function and, because the save is hoisted to the
   prologue, one push/pop pair covers both return paths. That reproduces the
   target exactly.

   MINIMAL winning set: ONE pragma.
     #pragma aux alloc_seq_slot modify [eax ecx edx ebx];
   The two FUN_* symbols are address-of operands (passed as pointers, never
   called) so pragmas on them do nothing. Of the six actually-called routines
   (alloc_seq_slot, timer_rate_critsec, start_seq, d_getvec, outp, d_setvec) any
   single one triggers the hoisted save and gives a full match, so the byte image
   cannot pin down which routine truly clobbers EBX. We declare just one. We pick
   alloc_seq_slot because it is the first real call, on the primary sound path,
   and a non-leaf allocator is the most plausible routine to trash EBX as scratch
   without restoring it. The eax/ecx/edx in the list are the ordinary scratch
   registers a stack-based Watcom call already clobbers, so the declaration is
   truthful. This only ADDS to the clobber set and never claims any callee
   preserves a register. */
extern unsigned char g_sound_enabled, g_music_enabled;
extern int g_defc;

extern void FUN_00017a70(void);
extern void FUN_00017a90(void);

extern int  alloc_seq_slot(void *p);
extern void timer_rate_critsec(int a, int b);
extern void start_seq(int a);

extern void __far *d_getvec(unsigned n);
extern void outp(int a, int b);
extern void d_setvec(int a, void __far *p);

extern void __far *g_df08;   /* off @0xdf08, seg @0xdf0c */

/* alloc_seq_slot clobbers EBX (see header) -- forces the target's hoisted,
   otherwise-dead push ebx / pop ebx around the whole body. */
#pragma aux alloc_seq_slot modify [eax ecx edx ebx];

void guarded_init_alloc(void)
{
    if (g_sound_enabled != 0 || g_music_enabled != 0) {
        g_defc = alloc_seq_slot((void *)FUN_00017a70);
        if (g_defc != -1) {
            timer_rate_critsec(g_defc, 0x48);
            start_seq(g_defc);
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
