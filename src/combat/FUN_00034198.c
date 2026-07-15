/* 0x34198 -- march a shot along its direction for `count` steps, running
 * collision on the first 3 steps and then applying damage to `p` based on
 * whether/where a wall/turn was hit.
 *
 * setup: dir = p2[0x1a] (facing); hit = -1 (turn tracker). Phase-1 loop marches
 * the shot cursor (g_shot_x x, g_shot_y y) via the direction tables g_dir_dx/g_dir_dy
 * (SHL 8 = *0x100, >>8), re-picking the passable direction each step with
 * FUN_34608. On the first turn (dir != orig) it records hit = i. For i<3 it
 * queries collision FUN_34088; on a hit it damages p (type-2 node -> FUN_34118,
 * else, if p not flagged, FUN_34168), sets hit = 0x3e7 and breaks to the phase-2
 * continuation loop (same march, no collision). Tail: if the turn landed inside
 * the range apply FUN_34168+floor p[0x54] at p[0x55]/2; if no turn ever happened
 * bump p[0x54] += 4 clamped to p[0x55]. Recipe: -4s -oneatx -zp8 -s -zq
 *
 * PARKED near-miss, ~95.4% (453/453 B, exact length; first diff 0x77). Semantics
 * byte-correct: setup, phase-1 march-1, the whole collision/damage dispatch, the
 * phase-2 continuation head, and the entire tail all match byte-for-byte. Levers
 * that landed: FUN_34608 param declared `short` -> the `xor;mov al;cwde` arg
 * widen; the tail floor compare written `p[0x54] < p[0x55]/2` -> target's
 * `cmp edx,eax; jge` orientation; and `volatile short g_shot_y` to stop the
 * phase-1 scheduler interleaving the two marches.
 *
 * WALL (loop-split scheduling tie): the target emitted TWO byte-identical physical
 * copies of the march (phase-1 with collision, phase-2 continuation after the
 * break). In BOTH copies the y-march is the clean per-statement schedule
 * `shl edx,8; movsx eax,g_shot_y; sar edx,8` (the g_shot_y load hoisted into the
 * shl/sar latency gap). Our phase-2 reproduces this with a plain non-volatile
 * `+=`; but phase-1's trailing FUN_34088 collision CALL frees EAX/ECX/EDX and lets
 * -oneatx interleave the two marches -- it loads g_dir_dy straight into EDX
 * (`movsx edx,[edx*2+..]`, 8 B) instead of `movsx eax; mov edx,eax` (10 B), 2 B
 * short. `volatile g_shot_y` blocks that interleave (restores exact length + the
 * `mov edx,eax` form) but, being a hard scheduling barrier, pins the g_shot_y load
 * to program order AFTER `sar edx,8` in BOTH copies -- one instruction late vs the
 * target's gap-fill. So the g_shot_y load is either hoisted-but-interleaved (plain
 * +=, phase-2 exact / phase-1 interleaved, 451 B) or barrier-pinned-late (volatile,
 * 453 B both copies 1 instr off). No source form gives BOTH the gap hoist AND no
 * interleave, because the gap-fill needs a reorderable (non-volatile) load and the
 * anti-interleave needs a barrier (volatile) -- mutually exclusive here. Tried:
 * named delta temp (word-arith regression), int/short split temps (spill, 461 B),
 * volatile-alias in phase-1 only (interleave returns -- the barrier needs the SAME
 * symbol volatile in both copies to interact with the call's g_shot_y read), -or
 * recipe (loop layout diverges at 0x7). Genuine phase-context scheduling wall of
 * the "two loop copies need opposite decisions" class (cf. FUN_34608 cross-jump).
 */
extern volatile short g_shot_y;
extern short g_shot_x;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern unsigned short FUN_00034608(short dir);
extern unsigned char *FUN_00034088(void);
extern void FUN_00034118(unsigned char *dst, unsigned char *src, unsigned short thr);
extern void FUN_00034168(unsigned char *p, unsigned short n);

void FUN_00034198(unsigned char *p2, unsigned char *p, unsigned short count)
{
    unsigned char dir = p2[0x1a];
    unsigned char orig = dir;
    short hit = -1;
    short i;
    unsigned char *node;

    for (i = 0; i < count; i++) {
        dir = (unsigned char)FUN_00034608(dir);
        if (dir != orig && hit == -1)
            hit = i;
        g_shot_x += g_dir_dx[dir] * 0x100 >> 8;
        g_shot_y += g_dir_dy[dir] * 0x100 >> 8;
        if (i < 3) {
            node = FUN_00034088();
            if (node != 0) {
                if (node[0x18] == 2) {
                    if (i < 1)
                        p[0x54] = 0;
                    else
                        FUN_00034118(p, node, 8);
                    hit = 0x3e7;
                    break;
                }
                if (p[0x1c] & 2)
                    continue;
                if (i < 1)
                    p[0x54] = 0;
                else
                    FUN_00034168(p, 0x10);
                hit = 0x3e7;
                break;
            }
        }
    }
    for (; i < count; i++) {
        dir = (unsigned char)FUN_00034608(dir);
        if (dir != orig && hit == -1)
            hit = i;
        g_shot_x += g_dir_dx[dir] * 0x100 >> 8;
        g_shot_y += g_dir_dy[dir] * 0x100 >> 8;
    }

    if (hit >= count)
        goto no_turn;
    if (hit < 0)
        goto no_turn;
    FUN_00034168(p, 8);
    if (p[0x54] < p[0x55] / 2)
        p[0x54] = (unsigned char)(p[0x55] / 2);
    goto done;
no_turn:
    if (hit == -1) {
        p[0x54] += 4;
        if (p[0x54] > p[0x55])
            p[0x54] = p[0x55];
    }
done:
    ;
}
