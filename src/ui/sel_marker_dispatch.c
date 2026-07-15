/* @ 0x1bc28: 10-way selection-marker dispatcher (inline jump-table switch).
 * sel = param_1 (cases 1..10 -> dec/cmp 9/ja default). Jump table at obj1:+0xe4b0
 * (manifest 0x1bbf8), decoded via tools/lefix.py:
 *   sel1->0x1bdf6(SW)  sel2->0x1bd88(NE)  sel3->default  sel4->0x1bd03(NW)
 *   sel5->0x1be68(W)   sel6->0x1bf1c(N)   sel7->default  sel8->0x1bc6a(SE)
 *   sel9->0x1bfb9(S)   sel10->0x1c083(E)
 * So the PHYSICAL body order (== source order) is 8,4,2,1,5,6,9,10; cases 3 and 7
 * are empty (default). The 4 diagonal "corner" cases (SE/NW/NE/SW) move the cursor
 * (g_map_cursor_x,g_map_cursor_y) one step and draw one marker; if a bound blocks the diagonal they
 * `goto` an adjacent "edge" case (W/N/S/E) that steps 2 along an axis and draws two.
 * g_marker_phase_a/g_marker_phase_b are cyclic phase counters (0..0x18 / 0..0xf).  The four inline
 * neighbour expressions (g_map_cursor_x+-1, g_map_cursor_y+-1) are each reused across two corner
 * cases -> Watcom CSE-hoists them into the switch preamble (ecx=y+1/esi=x+1/edx=x-1/
 * ebx=y-1, AFTER the dec/cmp/ja range check).  NE/N/E share one FUN_00045e61 call
 * tail (identical args) via -oneatx cross-jump; SW joins only the final push+call.
 * Cousin: src/clamp_point_box.c.
 *
 * STATUS: NEAR-MISS +2 bytes (1336 vs true 1334, code-only; the compiled obj carries a
 * co-located [table 40B][pad 8B] prefix that match_reloc strips, so the tool prints
 * ours=1384/target=1334 and a bogus "first diff 0x28" from the size-mismatched tail
 * split).  Structure is fully recovered: case->body map, jump-table decode, the 4-way
 * neighbour CSE hoist, and the NE/N/E cross-jump tail-merge all reproduce.  The residual
 * +2 is a register-allocation tie-break in the edge cases (W/N/S/E): the target does the
 * `g +- 2` update IN-PLACE on the accumulator loaded by the guard compare and stores it
 * with the compact `a3` form (sub eax,2; mov moffs,ax), whereas Watcom 9.5b copies to a
 * fresh callee-saved reg first (mov ecx,eax; sub ecx,2; mov [g],cx, +? bytes) even though
 * that reg is dead after the store.  Volatile-alias re-read (forced the movsx re-reads for
 * the draw-call args but not the store form), compound `g -= 2`, and recipes -ot/-os/-oe/-or
 * all left the same +2.  Genuine allocator wall (playbook 3). */
extern unsigned short g_map_cursor_x, g_map_cursor_y;
extern short g_1be32, g_1be34, g_1be36, g_1be38;
extern int g_marker_phase_a, g_marker_phase_b;
extern int FUN_00045f8a();
extern int FUN_00045e61();
extern void fill_minimap_grid(void);

void sel_marker_dispatch(short sel)
{
    unsigned short kA, kB;

    switch (sel) {
    case 8: /* SE */
        if ((short)g_map_cursor_x >= g_1be36) goto edge_S;
        if ((short)g_map_cursor_y >= g_1be38) goto edge_E;
        g_map_cursor_x = g_map_cursor_x + 1; g_map_cursor_y = g_map_cursor_y + 1;
        if (g_marker_phase_a == 0x18) { g_marker_phase_a = 0; kA = 0x18; }
        else { g_marker_phase_a = g_marker_phase_a + 1; kA = g_marker_phase_a - 1; }
        FUN_00045f8a((short)(g_map_cursor_x + 0x18), (short)(g_map_cursor_y + 0x18), 0x10, g_marker_phase_b, kA);
        fill_minimap_grid();
        return;
    case 4: /* NW */
        if ((short)g_map_cursor_x <= g_1be32) goto edge_N;
        if ((short)g_map_cursor_y <= g_1be34) goto edge_W;
        g_map_cursor_x = g_map_cursor_x - 1; g_map_cursor_y = g_map_cursor_y - 1;
        if (g_marker_phase_a == 0) g_marker_phase_a = 0x18; else g_marker_phase_a = g_marker_phase_a - 1;
        FUN_00045f8a((short)g_map_cursor_x, (short)g_map_cursor_y, 0x10, g_marker_phase_b, g_marker_phase_a);
        fill_minimap_grid();
        return;
    case 2: /* NE */
        if ((short)g_map_cursor_x >= g_1be36) goto edge_N;
        if ((short)g_map_cursor_y <= g_1be34) goto edge_E;
        g_map_cursor_x = g_map_cursor_x + 1; g_map_cursor_y = g_map_cursor_y - 1;
        if (g_marker_phase_b == 0xf) { g_marker_phase_b = 0; kB = 0xf; }
        else { kB = g_marker_phase_b; g_marker_phase_b = g_marker_phase_b + 1; }
        FUN_00045e61((short)(g_map_cursor_x + 0xf), (short)(g_map_cursor_y - 0xf), 0x19, kB, g_marker_phase_a);
        break;
    case 1: /* SW */
        if ((short)g_map_cursor_x <= g_1be32) goto edge_S;
        if ((short)g_map_cursor_y >= g_1be38) goto edge_W;
        g_map_cursor_x = g_map_cursor_x - 1; g_map_cursor_y = g_map_cursor_y + 1;
        if (g_marker_phase_b == 0) g_marker_phase_b = 0xf; else g_marker_phase_b = g_marker_phase_b - 1;
        FUN_00045e61((short)g_map_cursor_x, (short)g_map_cursor_y, 0x19, g_marker_phase_b, g_marker_phase_a);
        break;
    case 5: edge_W: /* W */
        if ((short)g_map_cursor_x <= g_1be32) break;
        g_map_cursor_x = g_map_cursor_x - 2;
        if (g_marker_phase_a == 0) g_marker_phase_a = 0x18; else g_marker_phase_a = g_marker_phase_a - 1;
        if (g_marker_phase_b == 0) g_marker_phase_b = 0xf; else g_marker_phase_b = g_marker_phase_b - 1;
        FUN_00045e61((short)g_map_cursor_x, (short)g_map_cursor_y, 0x19, g_marker_phase_b, g_marker_phase_a);
        FUN_00045f8a((short)g_map_cursor_x, (short)g_map_cursor_y, 0x10, g_marker_phase_b, g_marker_phase_a);
        fill_minimap_grid();
        return;
    case 6: edge_N: /* N */
        if ((short)g_map_cursor_y <= g_1be34) break;
        g_map_cursor_y = g_map_cursor_y - 2;
        if (g_marker_phase_a == 0) g_marker_phase_a = 0x18; else g_marker_phase_a = g_marker_phase_a - 1;
        if (g_marker_phase_b == 0xf) { g_marker_phase_b = 0; kB = 0xf; }
        else { kB = g_marker_phase_b; g_marker_phase_b = g_marker_phase_b + 1; }
        FUN_00045f8a((short)g_map_cursor_x, (short)g_map_cursor_y, 0x10, g_marker_phase_b, g_marker_phase_a);
        FUN_00045e61((short)(g_map_cursor_x + 0xf), (short)(g_map_cursor_y - 0xf), 0x19, kB, g_marker_phase_a);
        break;
    case 9: edge_S: /* S */
        if ((short)g_map_cursor_y >= g_1be38) break;
        g_map_cursor_y = g_map_cursor_y + 2;
        if (g_marker_phase_a == 0x18) { g_marker_phase_a = 0; kA = 0x18; }
        else { g_marker_phase_a = g_marker_phase_a + 1; kA = g_marker_phase_a - 1; }
        if (g_marker_phase_b == 0) g_marker_phase_b = 0xf; else g_marker_phase_b = g_marker_phase_b - 1;
        FUN_00045f8a((short)(g_map_cursor_x + 0x18), (short)(g_map_cursor_y + 0x18), 0x10, g_marker_phase_b, kA);
        FUN_00045e61((short)g_map_cursor_x, (short)g_map_cursor_y, 0x19, g_marker_phase_b, g_marker_phase_a);
        fill_minimap_grid();
        return;
    case 10: edge_E: /* E */
        if ((short)g_map_cursor_x >= g_1be36) break;
        g_map_cursor_x = g_map_cursor_x + 2;
        if (g_marker_phase_a == 0x18) { g_marker_phase_a = 0; kA = 0x18; }
        else { g_marker_phase_a = g_marker_phase_a + 1; kA = g_marker_phase_a - 1; }
        if (g_marker_phase_b == 0xf) { g_marker_phase_b = 0; kB = 0xf; }
        else { kB = g_marker_phase_b; g_marker_phase_b = g_marker_phase_b + 1; }
        FUN_00045f8a((short)(g_map_cursor_x + 0x18), (short)(g_map_cursor_y + 0x18), 0x10, g_marker_phase_b, kA);
        FUN_00045e61((short)(g_map_cursor_x + 0xf), (short)(g_map_cursor_y - 0xf), 0x19, kB, g_marker_phase_a);
        break;
    }
    fill_minimap_grid();
}
