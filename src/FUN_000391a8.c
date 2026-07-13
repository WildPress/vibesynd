/* FUN_000391a8 (cdecl/-4s). Scan the key/command record list starting at the
 * saved cursor g_5398, dispatching each record whose word0 matches g_bde4.
 * Records are 4 bytes: [word0 key][byte2 cmd][byte3 arg]. Commands:
 *   'A'/'a' -> g_bdf8 = arg
 *   'M'/'m' -> FUN_00038fe8((signed char)arg), preserving the cursor across it
 *   'S'/'s' -> if (g_10b4a) FUN_00035f78(arg), preserving the cursor
 * The cursor g_5398 is written before each call (the callee may read/advance it)
 * and re-read afterwards. The record address base+si*4 is recomputed at each use.
 *
 * NEAR-MISS (208/216, recipe -4s -oneatx, first diff at 0x2). The control flow,
 * the inlined per-use address recompute, the `int si` cursor promoted to a
 * callee-saved reg + incremented `inc esi` and used via `movsx` (short cast),
 * the save/reload of g_5398 around the calls, and all relocs are byte-correct.
 * Residual: a register-role tie-break. The target allocates the cursor si to ESI
 * and base to EBX (loading si with a bare 16-bit `mov si,[g_5398]`); ours does
 * the reverse (base->ESI, si->EBX via `movsx ebx`). ESI is the first
 * callee-saved reg Watcom hands out and it gives it to base (loop-invariant)
 * where the target gives it to si (loop-carried). Decl-order reorder, a base
 * local copy, and -ot/-oe/-os recipes all leave the swap in place. §3
 * register-role wall. */
extern short         g_5398;
extern unsigned short g_bde4;
extern unsigned char g_10b4a;
extern int           g_bdf8;
extern void FUN_00035f78(int);
extern void FUN_00038fe8(int);

void FUN_000391a8(unsigned char *base)
{
    int si;

    si = g_5398;
    for (;;) {
        if (g_bde4 != *(unsigned short *)(base + (short)si * 4))
            break;
        switch ((base + (short)si * 4)[2]) {
        case 'A':
        case 'a':
            g_bdf8 = (base + (short)si * 4)[3];
            break;
        case 'M':
        case 'm':
            g_5398 = (short)si;
            FUN_00038fe8((signed char)(base + (short)si * 4)[3]);
            si = g_5398;
            break;
        case 'S':
        case 's':
            if (g_10b4a != 0) {
                g_5398 = (short)si;
                FUN_00035f78((base + (short)si * 4)[3]);
                si = g_5398;
            }
            break;
        default:
            break;
        }
        si++;
    }
    g_5398 = si;
}
