/* @ 0x1bc28 (size 0x535 = 1333B): 10-way selection-marker dispatcher.
 *
 * Prologue: push ebx/esi/edi/ebp; sel = (unsigned short)(param_1 - 1);
 * if (sel >= 10) { FUN_0001a8c8(); return; }   // default = grid-fill + return
 * else precompute the 4 neighbours of the current cursor point:
 *     esi = g_0000 + 1;  edx = g_0000 - 1;   (x+1 / x-1)
 *     ecx = g_0002 + 1;  ebx = g_0002 - 1;   (y+1 / y-1)
 * then  JMP dword ptr CS:[sel*4 + 0xe4b0].
 * Each of the 10 cases draws a different marker style around (g_0000,g_0002) via
 * FUN_00045f8a / FUN_00045e61 (pushing the neighbour coords, 0x19, and the
 * g_10ab4/g_10ab8 accumulators), then falls into the shared tail
 *     CALL FUN_0001a8c8; pop ebp/edi/esi/ebx; ret.
 *
 * WALL (inline switch jump-table — same class as 0x23038, playbook 0.):
 * In the shipped binary the dispatch table lives in a FAR segment
 * (`CS:[eax*4 + 0xe4b0]`, unreadable from .text) and the on-disk function body is
 * clean code only. Watcom 9.5b, compiling ANY C `switch`, CO-LOCATES the generated
 * jump table (+ entry-alignment pad) inside this object's own .text, so the compiled
 * object length can never equal the target's clean code window and match_reloc always
 * reports a length/masked mismatch even when every case body is byte-identical. No C
 * spelling (goto ladders, function-pointer table, nested if/else) moves the table out
 * of the compiled object. Not source-reachable — documented, not matched.
 */
