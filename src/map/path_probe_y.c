/* @ 0x2d468 -- twin of 0x2d5b8 (path/passability probe). 16-entry jump table at
   manifest 0x2d428 (obj1:+0x1fce0 literal, decoded via lefix.py): blocked tile
   set = {0,6,7,8,9,0xb,0xf} (the twin's set PLUS type 0), open = the rest.
   Watcom jump-threads the f=1 cases straight to the fa18 arm. Differences from
   the twin: threshold +-0x20 (not 0x40) and a flag escape -- returns 1 when
   |g_level_step diff| <= 0x20, or when below -0x20 but p[0xb] bit1 is set.
   Uses the cont.21 g_map_cols lever set: pointer-variable decl, slot local,
   (int)*slot deref cast, ternary call merge, volatile g_level_step + named t.

   PARKED at the SAME ~10-byte residue as the twin 0x2d5b8 (jump-table verified
   layout, entry/guards/switch-dispatch/calls byte-correct): slot addr via
   `add ecx,eax` (target `lea ecx,[edx+eax]` + early movsx), tile widen and-form
   in EAX (target xor/mov dl in EDX), plus here the t reload lands AX (target
   DX). Named-base variant regresses (338B). If the 0x33fb8 g_map_cols retry finds
   the missing spelling, transplant it to BOTH twins. */
extern char **g_map_cols;
extern unsigned char *g_tile_flags;
extern volatile short g_level_step;
extern int FUN_LE_0000fa18(int a, int b, int c);
extern int FUN_LE_0000fa88(int a, int b, int c);

int path_probe_y(short x, short y, int w, unsigned char *p)
{
    short f = 0;
    short t;
    char **slot;

    if (!(p[0x1c] & 2) && !(p[0x1d] & 8)) {
        slot = g_map_cols + ((*(short *)(p + 6) % 0x6000 / 0x100 << 7)
                        + (*(short *)(p + 4) & 0xff00) / 0x100);
        switch (g_tile_flags[*(unsigned char *)((int)*slot
                                           + (*(short *)(p + 8) - 1) / 0x80)]) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 0xa:
        case 0xc:
        case 0xd:
        case 0xe:
            break;
        case 0:
        case 6:
        case 7:
        case 8:
        case 9:
        case 0xb:
        case 0xf:
            f = 1;
            break;
        }
    } else
        f = 1;
    g_level_step = (f != 0 ? FUN_LE_0000fa18(x, y, (short)w) : FUN_LE_0000fa88(x, y, (short)w)) - w;
    t = g_level_step;
    if (t <= 0x20 && (t >= -0x20 || (p[0xb] & 2)))
        return 1;
    return 0;
}
