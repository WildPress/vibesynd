/* walk_sound_record_table @ 0x35e68 - walk the 0x20-stride record table (g_objective_slots..g_5354),
 * from the 2nd record on; for each record add g_534c to its +0x12 dword, store
 * that <<12 into +0xe, write the sum back to +0x12, then dispatch via 0x39af1
 * (opcode 0x85) with the DS segment. Guarded by pointer!=0 && g_sound_enabled flag.
 * Stack-calling (-4s).
 *
 * NEAR-MISS / WALL (§3 register-role tie-break). 103/103 bytes; the entire
 * instruction stream is byte-identical EXCEPT a stable 3-way register rotation
 * that no source spelling or operand orientation moves:
 *   role         target   ours
 *   p            ESI      EBX
 *   g_534c       ECX      ESI
 *   guard bound  EBX      ECX
 * The (void __far*)v cast correctly reproduces the mov dx,ds / and edx,0xffff /
 * push DS idiom. First diff at 0x18 (mov ebx vs mov ecx). Every C form + operand
 * flip converge to the same wrong colouring -> allocator tie-break, not source-
 * reachable. Block/reg-permuter may close it. */
extern unsigned char *g_objective_slots, *g_5354;
extern int g_534c, g_11dec;
extern unsigned char g_sound_enabled;
extern void FUN_00039af1(int a, void __far *p, int c);

void walk_sound_record_table(void)
{
    unsigned char *p;
    int v;

    if (g_objective_slots != 0 && g_sound_enabled != 0 && g_5354 > (p = g_objective_slots + 0x20)) {
        do {
            v = *(int *)(p + 0x12) + g_534c;
            *(int *)(p + 0xe) = v * 0x1000;
            *(int *)(p + 0x12) = v;
            FUN_00039af1(g_11dec, (void __far *)v, -1);
            p += 0x20;
        } while (g_5354 > p);
    }
}
