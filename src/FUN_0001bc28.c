/* @ 0x1bc28: 10-way selection-marker dispatcher (inline jump-table switch).
 * sel = param_1 (cases 1..10 -> dec/cmp 9/ja default). Jump table at obj1:+0xe4b0
 * (manifest 0x1bbf8), decoded via tools/lefix.py:
 *   sel1->0x1bdf6(SW)  sel2->0x1bd88(NE)  sel3->default  sel4->0x1bd03(NW)
 *   sel5->0x1be68(W)   sel6->0x1bf1c(N)   sel7->default  sel8->0x1bc6a(SE)
 *   sel9->0x1bfb9(S)   sel10->0x1c083(E)
 * So the PHYSICAL body order (== source order) is 8,4,2,1,5,6,9,10; cases 3 and 7
 * are empty (default). The 4 diagonal "corner" cases (SE/NW/NE/SW) move the cursor
 * (g_0000,g_0002) one step and draw one marker; if a bound blocks the diagonal they
 * `goto` an adjacent "edge" case (W/N/S/E) that steps 2 along an axis and draws two.
 * g_10ab4/g_10ab8 are cyclic phase counters (0..0x18 / 0..0xf).  The four inline
 * neighbour expressions (g_0000+-1, g_0002+-1) are each reused across two corner
 * cases -> Watcom CSE-hoists them into the switch preamble (ecx=y+1/esi=x+1/edx=x-1/
 * ebx=y-1, AFTER the dec/cmp/ja range check).  NE/N/E share one FUN_00045e61 call
 * tail (identical args) via -oneatx cross-jump; SW joins only the final push+call.
 * Cousin: src/FUN_0001ba48.c. */
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
    case 8: /* SE */
        if ((short)g_0000 >= g_1be36) goto edge_S;
        if ((short)g_0002 >= g_1be38) goto edge_E;
        g_0000 = g_0000 + 1; g_0002 = g_0002 + 1;
        if (g_10ab4 == 0x18) { g_10ab4 = 0; kA = 0x18; }
        else { g_10ab4 = g_10ab4 + 1; kA = g_10ab4 - 1; }
        FUN_00045f8a((short)(g_0000 + 0x18), (short)(g_0002 + 0x18), 0x10, g_10ab8, kA);
        FUN_0001a8c8();
        return;
    case 4: /* NW */
        if ((short)g_0000 <= g_1be32) goto edge_N;
        if ((short)g_0002 <= g_1be34) goto edge_W;
        g_0000 = g_0000 - 1; g_0002 = g_0002 - 1;
        if (g_10ab4 == 0) g_10ab4 = 0x18; else g_10ab4 = g_10ab4 - 1;
        FUN_00045f8a((short)g_0000, (short)g_0002, 0x10, g_10ab8, g_10ab4);
        FUN_0001a8c8();
        return;
    case 2: /* NE */
        if ((short)g_0000 >= g_1be36) goto edge_N;
        if ((short)g_0002 <= g_1be34) goto edge_E;
        g_0000 = g_0000 + 1; g_0002 = g_0002 - 1;
        if (g_10ab8 == 0xf) { g_10ab8 = 0; kB = 0xf; }
        else { kB = g_10ab8; g_10ab8 = g_10ab8 + 1; }
        FUN_00045e61((short)(g_0000 + 0xf), (short)(g_0002 - 0xf), 0x19, kB, g_10ab4);
        break;
    case 1: /* SW */
        if ((short)g_0000 <= g_1be32) goto edge_S;
        if ((short)g_0002 >= g_1be38) goto edge_W;
        g_0000 = g_0000 - 1; g_0002 = g_0002 + 1;
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
    case 9: edge_S: /* S */
        if ((short)g_0002 >= g_1be38) break;
        g_0002 = g_0002 + 2;
        if (g_10ab4 == 0x18) { g_10ab4 = 0; kA = 0x18; }
        else { g_10ab4 = g_10ab4 + 1; kA = g_10ab4 - 1; }
        if (g_10ab8 == 0) g_10ab8 = 0xf; else g_10ab8 = g_10ab8 - 1;
        FUN_00045f8a((short)(g_0000 + 0x18), (short)(g_0002 + 0x18), 0x10, g_10ab8, kA);
        FUN_00045e61((short)g_0000, (short)g_0002, 0x19, g_10ab8, g_10ab4);
        FUN_0001a8c8();
        return;
    case 10: edge_E: /* E */
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
