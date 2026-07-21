/* walk_sound_record_table @ 0x35e68 - walk the 0x20-stride record table (g_objective_slots..g_objective_slots_end),
 * from the 2nd record on; for each record add g_534c to its +0x12 dword, store
 * that <<12 into +0xe, write the sum back to +0x12, then dispatch via 0x39af1
 * (opcode 0x85) with the DS segment. Guarded by pointer!=0 && g_sound_enabled flag.
 * Stack-calling (-4s).
 *
 * MATCHED (RELOC-AWARE YES). Recipe: -4s -oneatx -zp8 -s -zq. Also matches with
 * -oaxt / -oneatxi / -oneatxe / -zp1 / -zp4 and -5s; NOT with -ox/-os/-ot/-4r/-3s.
 * 103/103 bytes; register roles now byte-identical.
 *
 * WHAT CRACKED IT (register-role rotation, previously called a wall): the entire
 * instruction stream was already byte-identical EXCEPT a stable 3-way register
 * rotation {p, g_534c, guard-bound} that no source spelling or operand flip moved:
 *   role         target   ours(before)
 *   p            ESI      EBX
 *   g_534c       ECX      ESI
 *   guard bound  EBX      ECX
 * The rotation is driven by which registers the sound dispatch is assumed to
 * clobber, which reshapes the interference-graph colouring. Declaring the
 * callee's modify set forces the target's colouring:
 *   #pragma aux snd_cmd_85 modify [eax edx ebx ecx edi ebp];   (preserve ESI only)
 * This seats p->ESI, guard->EBX, g_534c->ECX exactly as the original build.
 * The (void __far*)v cast reproduces the mov dx,ds / and edx,0xffff / push DS
 * idiom. Found via the source x flag cross-product (modify-set is the source
 * lever; the -oneatx flag family is where it lands). */
extern unsigned char *g_objective_slots, *g_objective_slots_end;
extern int g_534c, g_snd_driver;
extern unsigned char g_sound_enabled;
extern void snd_cmd_85(int a, void __far *p, int c);
#pragma aux snd_cmd_85 modify [eax edx ebx ecx edi ebp];

void walk_sound_record_table(void)
{
    unsigned char *p;
    int v;

    if (g_objective_slots != 0 && g_sound_enabled != 0 && g_objective_slots_end > (p = g_objective_slots + 0x20)) {
        do {
            v = *(int *)(p + 0x12) + g_534c;
            *(int *)(p + 0xe) = v * 0x1000;
            *(int *)(p + 0x12) = v;
            snd_cmd_85(g_snd_driver, (void __far *)v, -1);
            p += 0x20;
        } while (g_objective_slots_end > p);
    }
}
