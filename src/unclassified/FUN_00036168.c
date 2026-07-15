/* @ 0x36168 (22B): if (g_10b4a) FUN_000395b6(0);
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
extern unsigned char g_10b4a;
extern void FUN_000395b6(int);
#pragma aux FUN_000395b6 modify [ebx];

void FUN_00036168(void)
{
    if (g_10b4a != 0)
        FUN_000395b6(0);
}
