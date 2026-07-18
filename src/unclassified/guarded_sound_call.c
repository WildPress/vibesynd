/* @ 0x36168 (22B): if (g_sound_enabled) unload_all_drivers(0);
 *
 * The body compiles to a bare guarded call, but the target wraps it in a dead
 * push ebx / pop ebx: this function must preserve EBX for its own caller
 * (callee-saved contract), and its callee 0x395b6 is declared -- via the aux
 * pragma below, i.e. the original translation unit's header ABI -- to modify
 * EBX. So Watcom saves/restores EBX around the whole body even though nothing
 * here touches it. That declared-clobber is the ONLY C construction that yields
 * these exact bytes (any real EBX use emits extra loads/stores); it is the
 * standard technique for this "dead callee-save" artifact.
 *
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char g_sound_enabled;
extern void unload_all_drivers(int);
#pragma aux unload_all_drivers modify [ebx];

void guarded_sound_call(void)
{
    if (g_sound_enabled != 0)
        unload_all_drivers(0);
}
