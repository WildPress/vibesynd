/* NEAR-MISS @ 0x2d5b8 -- EDIT-DIST 107 (was 120). The whole prologue, guards,
 * column lookup, switch dispatch and calls are now byte-identical to target.
 *
 * KEY FIX this pass: load the tile byte into its own `unsigned char tile` local
 * BEFORE the switch, instead of inlining the deref inside g_tile_flags[...].
 * That single change reschedules the column lookup into the target's exact shape:
 * g_map_cols base into EDX, slot address via `lea ecx,[edx+eax]`, and the
 * `movsx edx,[ebx+8]` for the level scheduled AFTER the slot is formed. This was
 * the old "register-role wall" (base ECX + add, movsx early); the tile local
 * crosses it cleanly. Semantics unchanged: tile is 0..255, index into g_tile_flags.
 *
 * REMAINING (~6 code bytes, two register-role ties, not source-reachable):
 *   (1) tile-byte widen: ours `mov al,[eax]; and eax,0xff` with g_tile_flags in
 *       EDX; target `xor edx,edx; mov dl,[eax]` with g_tile_flags in EAX. Classic
 *       xor-clear vs and-mask tie. Tried int/uchar tile, base-ptr local, flag
 *       local, address local, array-index form -- all regress or leave it.
 *   (2) tail: target does `sub eax,esi` before `add esp,0xc` and reloads the
 *       volatile g_level_step into DX (then movsx eax,dx); ours cleans the stack
 *       first and reloads into AX (then cwde). Order + reload-register schedule tie.
 * These two make ours 260 code bytes vs target 259, so the harness JUMP-TABLE
 * alignment (code == exact tail) also can't latch yet. TWIN 0x2d468 (path_probe_y)
 * shares this exact idiom and floor.
 *
   0x2d5b8 -- path/passability probe at (x,y,w) for object p. Unless p is
 * flagged (+0x1c bit 1 / +0x1d bit 3), looks up the tile type under p
 * (column table g_map_cols, level (w8-1)/0x80, translated through the g_tile_flags
 * pointer table) and switches on it: types {6,7,8,9,0xb,0xf} are blocked
 * (case map recovered via tools/lefix.py from the 16-entry jump table at
 * manifest 0x2d574 -- obj1:+0x1fe2c). Blocked -> 0xfa18(x,y,w), open ->
 * 0xfa88(x,y,w) (both in the cut-off obj1 prefix); stores the result - w
 * into g_level_step and returns 1 iff it is within [-0x40, 0x40].
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char **g_map_cols;
extern unsigned char *g_tile_flags;
extern volatile short g_level_step;
extern int FUN_LE_0000fa18(int a, int b, int c);
extern int FUN_LE_0000fa88(int a, int b, int c);

int FUN_0002d5b8(short x, short y, int w, unsigned char *p)
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
        case 0:
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
    if (t <= 0x40 && t >= -0x40)
        return 1;
    return 0;
}
