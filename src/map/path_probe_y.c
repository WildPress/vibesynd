/* @ 0x2d468 -- twin of 0x2d5b8 (path/passability probe). EDIT-DIST 108 (was 121).
   16-entry jump table at manifest 0x2d428 (obj1:+0x1fce0 literal, decoded via
   lefix.py): blocked tile set = {0,6,7,8,9,0xb,0xf} (the twin's set PLUS type 0),
   open = the rest. Watcom jump-threads the f=1 cases straight to the fa18 arm.
   Differences from the twin: threshold +-0x20 (not 0x40) and a flag escape --
   returns 1 when |g_level_step diff| <= 0x20, or when below -0x20 but p[0xb]
   bit1 is set.

   KEY FIX this pass (same as the twin): the tile byte goes into its own
   `unsigned char tile` local before the switch. That reschedules the g_map_cols
   column lookup into the target's exact shape (base EDX, slot via lea ecx,[edx+eax],
   movsx for the level after the slot), making the prologue, guards, lookup, switch
   dispatch and calls byte-identical to target. Semantics unchanged.

   REMAINING (~6 code bytes, two register-role ties, not source-reachable, shared
   with the twin): (1) tile widen ours `mov al; and eax,0xff` (base in EDX) vs
   target `xor edx; mov dl` (base in EAX) -- xor-clear vs and-mask tie; (2) tail
   does `add esp` before `sub eax,esi` and reloads volatile g_level_step into AX
   + cwde, where target subtracts first and reloads into DX + movsx eax,dx. These
   leave ours 266 code bytes vs target 265, so the harness jump-table alignment
   cannot latch. Same floor as twin 0x2d5b8. */
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
        {unsigned char tile = *(unsigned char *)((int)*slot
                                           + (*(short *)(p + 8) - 1) / 0x80);
        switch (g_tile_flags[tile]) {
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
        }}
    } else
        f = 1;
    g_level_step = (f != 0 ? FUN_LE_0000fa18(x, y, (short)w) : FUN_LE_0000fa88(x, y, (short)w)) - w;
    t = g_level_step;
    if (t <= 0x20 && (t >= -0x20 || (p[0xb] & 2)))
        return 1;
    return 0;
}
