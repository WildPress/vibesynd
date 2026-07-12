/* runtime-library flag setter @ 0x0003db36 (framed CLIB).
 * Body logic is byte-faithful (test [f+0xd]&0x20; call 0x3c44d([f+0x10]);
 * OR 0x20; if !([f+0xd]&7) OR 2). void return (no EAX set).
 *
 * WALL: prologue reg-save order. Target prologue is `push ebx; push ebp;
 * mov ebp,esp` (regs BEFORE frame, args at [EBP+0xc]). Watcom 9.5b with any
 * optimized recipe (-3s -of -oneatx and every -o* variant tried) emits the
 * frame-FIRST form `push ebp; mov ebp,esp; push ebx` instead; only -od gives
 * regs-first but de-optimizes the whole body and saves esi/edi too. This
 * cascades into different register allocation for the entire function, so it
 * diverges at byte 0. Same wall as unmatched siblings FUN_0003d3e4 / ca0d
 * (framed CLIB fns that save a callee reg). Plain framed fns (cfce, db69)
 * match with -3s -of. Not source-reachable. */
extern int FUN_0003c44d(int);

void FUN_0003db36(unsigned char *f)
{
    if ((f[0xd] & 0x20) == 0) {
        if (FUN_0003c44d(*(int *)(f + 0x10)) != 0) {
            f[0xd] |= 0x20;
            if ((f[0xd] & 7) == 0)
                f[0xd] |= 2;
        }
    }
}
