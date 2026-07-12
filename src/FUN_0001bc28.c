/* @ 0x1bc28: 10-way selection-marker dispatcher (inline jump-table switch).
 * sel = param_1 (cases 1..10 -> dec/cmp 9/ja default). 4 diagonal "corner" cases
 * (1..4) move the cursor (g_0000,g_0002) one step and draw one marker; if a bound
 * blocks the diagonal they fall to an adjacent "edge" case (5..8) that steps 2 along
 * an axis and draws two markers. g_10ab4/g_10ab8 are cyclic phase counters
 * (0..0x18 / 0..0xf; A-counter uses reread+dec form, B-counter captures old value).
 * Draws via FUN_00045f8a (0x10) / FUN_00045e61 (0x19).  Cousin: src/FUN_0001ba48.c.
 *
 * STATUS: structurally byte-faithful (control flow, immediates, call targets, and the
 * pre-switch 4-neighbour hoist ecx=y+1/esi=x+1/edx=x-1/ebx=y-1 all match the target).
 * TWO blockers to a verified match:
 *  (1) MANIFEST SIZE off-by-one: recorded 1333 excludes the function's final RET at
 *      0x1c15d.  True extent 0x1bc28..0x1c15d = 1334B.  With size 1333 the
 *      jump-table-aware tail split lands 1 byte into our code (skips push ebx), so it
 *      can never align.  Size must be 1334.
 *  (2) REGISTER-ROLE tie-break WALL (playbook 3): every case body is instruction-for-
 *      instruction identical to the target, but Watcom 9.5b assigns the counter
 *      temporaries to fresh regs (ebp/eax) instead of REUSING the dead hoisted-neighbour
 *      regs (edx/esi/edi) the original build reuses.  e.g. case2 dnA: target
 *      edi/ebp/edx/ebx vs ours esi/edi/eax/edx -- same 30 instrs, different regs.
 *      Not source-reachable: inline-hoist vs explicit locals, int vs unsigned short,
 *      statement reorder, and 6 recipes (-oneatx/-oe/-ot/-or/-os/-ol) all converge to
 *      the same (wrong) allocation. */
extern unsigned short g_0000, g_0002;
extern short g_1be32, g_1be34, g_1be36, g_1be38;
extern int g_10ab4, g_10ab8;
extern int FUN_00045f8a();
extern int FUN_00045e61();
extern void FUN_0001a8c8(void);

void FUN_0001bc28(short sel)
{
    unsigned short kA, kB;

    switch (sel) {
    case 1: /* SE */
        if ((short)g_0000 >= g_1be36) goto edge_S;
        if ((short)g_0002 >= g_1be38) goto edge_E;
        g_0002 = g_0002 + 1; g_0000 = g_0000 + 1;
        if (g_10ab4 == 0x18) { g_10ab4 = 0; kA = 0x18; }
        else { g_10ab4 = g_10ab4 + 1; kA = g_10ab4 - 1; }
        FUN_00045f8a((short)(g_0000 + 0x18), (short)(g_0002 + 0x18), 0x10, g_10ab8, kA);
        FUN_0001a8c8();
        return;
    case 2: /* NW */
        if ((short)g_0000 <= g_1be32) goto edge_N;
        if ((short)g_0002 <= g_1be34) goto edge_W;
        g_0002 = g_0002 - 1; g_0000 = g_0000 - 1;
        if (g_10ab4 == 0) g_10ab4 = 0x18; else g_10ab4 = g_10ab4 - 1;
        FUN_00045f8a((short)g_0000, (short)g_0002, 0x10, g_10ab8, g_10ab4);
        FUN_0001a8c8();
        return;
    case 3: /* NE */
        if ((short)g_0000 >= g_1be36) goto edge_N;
        if ((short)g_0002 <= g_1be34) goto edge_E;
        g_0002 = g_0002 - 1; g_0000 = g_0000 + 1;
        if (g_10ab8 == 0xf) { g_10ab8 = 0; kB = 0xf; }
        else { kB = g_10ab8; g_10ab8 = g_10ab8 + 1; }
        FUN_00045e61((short)(g_0000 + 0xf), (short)(g_0002 - 0xf), 0x19, kB, g_10ab4);
        break;
    case 4: /* SW */
        if ((short)g_0000 <= g_1be32) goto edge_S;
        if ((short)g_0002 >= g_1be38) goto edge_W;
        g_0002 = g_0002 + 1; g_0000 = g_0000 - 1;
        if (g_10ab8 == 0) g_10ab8 = 0xf; else g_10ab8 = g_10ab8 - 1;
        FUN_00045e61((short)g_0000, (short)g_0002, 0x19, g_10ab8, g_10ab4);
        break;
    case 5: edge_W: /* W */
        if ((short)g_0000 <= g_1be32) break;
        g_0000 = g_0000 - 2;
        if (g_10ab4 == 0) g_10ab4 = 0x18; else g_10ab4 = g_10ab4 - 1;
        if (g_10ab8 == 0) g_10ab8 = 0xf; else g_10ab8 = g_10ab8 - 1;
        FUN_00045e61((short)g_0000, (short)g_0002, 0x19, g_10ab8, g_10ab4);
        FUN_00045f8a((short)g_0000, (short)g_0002, 0x10, g_10ab8, g_10ab4);
        FUN_0001a8c8();
        return;
    case 6: edge_N: /* N */
        if ((short)g_0002 <= g_1be34) break;
        g_0002 = g_0002 - 2;
        if (g_10ab4 == 0) g_10ab4 = 0x18; else g_10ab4 = g_10ab4 - 1;
        if (g_10ab8 == 0xf) { g_10ab8 = 0; kB = 0xf; }
        else { kB = g_10ab8; g_10ab8 = g_10ab8 + 1; }
        FUN_00045f8a((short)g_0000, (short)g_0002, 0x10, g_10ab8, g_10ab4);
        FUN_00045e61((short)(g_0000 + 0xf), (short)(g_0002 - 0xf), 0x19, kB, g_10ab4);
        break;
    case 7: edge_S: /* S */
        if ((short)g_0002 >= g_1be38) break;
        g_0002 = g_0002 + 2;
        if (g_10ab4 == 0x18) { g_10ab4 = 0; kA = 0x18; }
        else { g_10ab4 = g_10ab4 + 1; kA = g_10ab4 - 1; }
        if (g_10ab8 == 0) g_10ab8 = 0xf; else g_10ab8 = g_10ab8 - 1;
        FUN_00045f8a((short)(g_0000 + 0x18), (short)(g_0002 + 0x18), 0x10, g_10ab8, kA);
        FUN_00045e61((short)g_0000, (short)g_0002, 0x19, g_10ab8, g_10ab4);
        FUN_0001a8c8();
        return;
    case 8: case 9: case 10: edge_E: /* E */
        if ((short)g_0000 >= g_1be36) break;
        g_0000 = g_0000 + 2;
        if (g_10ab4 == 0x18) { g_10ab4 = 0; kA = 0x18; }
        else { g_10ab4 = g_10ab4 + 1; kA = g_10ab4 - 1; }
        if (g_10ab8 == 0xf) { g_10ab8 = 0; kB = 0xf; }
        else { kB = g_10ab8; g_10ab8 = g_10ab8 + 1; }
        FUN_00045f8a((short)(g_0000 + 0x18), (short)(g_0002 + 0x18), 0x10, g_10ab8, kA);
        FUN_00045e61((short)(g_0000 + 0xf), (short)(g_0002 - 0xf), 0x19, kB, g_10ab4);
        break;
    }
    FUN_0001a8c8();
}
