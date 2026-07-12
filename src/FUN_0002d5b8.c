/* NEAR-MISS @ 0x2d5b8 -- 259 vs 264 code bytes; JUMP TABLE FULLY RECOVERED
 * (this is the payoff of the fixed tools/lefix.py). The 16-entry dispatcher at
 * manifest 0x2d574 (obj1:+0x1fe2c) maps tile type -> {6,7,8,9,0xb,0xf}=blocked.
 * The whole body is byte-identical through the switch EXCEPT the g_5358 column
 * lookup, which is the SAME register-role wall that parked 0x28ec8/0x33fb8:
 *   (1) target keeps &g_5358[idx] as a slot address and derefs in place
 *       (lea ecx,[edx+eax]; ... add eax,[ecx]); ours loads the pointer eagerly
 *       (mov ecx,[edx+eax]; add eax,ecx).
 *   (2) target reads the tile index into a PRE-CLEARED edx (xor edx; mov dl)
 *       and indexes g_10ac0 with base-in-EAX/idx-in-EDX; ours reads into AL,
 *       and eax,0xff, base-in-EDX. Classic xor-clear vs and-mask role tie-break.
 * char *col vs char **slot vs inline all converge to the eager-load/mask form.
 * Semantics 100%. TWIN 0x2d468 shares this exact idiom; decode via lefix.py.
 *
   0x2d5b8 -- path/passability probe at (x,y,w) for object p. Unless p is
 * flagged (+0x1c bit 1 / +0x1d bit 3), looks up the tile type under p
 * (column table g_5358, level (w8-1)/0x80, translated through the g_10ac0
 * pointer table) and switches on it: types {6,7,8,9,0xb,0xf} are blocked
 * (case map recovered via tools/lefix.py from the 16-entry jump table at
 * manifest 0x2d574 -- obj1:+0x1fe2c). Blocked -> 0xfa18(x,y,w), open ->
 * 0xfa88(x,y,w) (both in the cut-off obj1 prefix); stores the result - w
 * into g_e128 and returns 1 iff it is within [-0x40, 0x40].
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char **g_5358;
extern unsigned char *g_10ac0;
extern short g_e128;
extern int FUN_LE_0000fa18(int a, int b, int c);
extern int FUN_LE_0000fa88(int a, int b, int c);

int FUN_0002d5b8(short x, short y, short w, unsigned char *p)
{
    short f = 0;
    int r;

    if (!(p[0x1c] & 2) && !(p[0x1d] & 8)) {
        char *col = g_5358[(*(short *)(p + 6) % 0x6000 / 0x100 << 7)
                           + (*(short *)(p + 4) & 0xff00) / 0x100];
        switch (g_10ac0[col[(*(short *)(p + 8) - 1) / 0x80]]) {
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
        }
    } else
        f = 1;
    if (f != 0)
        r = FUN_LE_0000fa18(x, y, w);
    else
        r = FUN_LE_0000fa88(x, y, w);
    g_e128 = r - w;
    if (g_e128 > 0x40)
        return 0;
    if (g_e128 < -0x40)
        return 0;
    return 1;
}
