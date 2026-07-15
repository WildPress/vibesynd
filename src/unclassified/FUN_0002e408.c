/* NEAR-MISS @ 0x2e408 -- 231/240 (was 222/240); PARKED on the last x-product
 * register-role tie-break. cont.21 levers that CLOSED ground: (1) goto-fail
 * merge -- `goto fail;` from the quadrant guard into a `fail:` label INSIDE
 * the fuel-guard if-body reproduces the target's shared `xor eax; jmp` stub
 * between the guards and the tail (jne 0x12 into it), killing ours'
 * duplicated full epilogue; length now 240/240 and everything from the call
 * to the end matches except 2 bytes. (2) ADDEND-FIRST operand swap on the y
 * arg (`*(short*)(p+6) + (g_dir_dy[..]*p[0x54] >> 8)`) flips the y-product
 * ESI->ECX / w6 ECX->ESI 2-cycle to the target's allocation -- addend
 * position is value-numbering-significant where the multiply commute is
 * inert. The SAME swap on the x arg does NOT flip its cycle (it instead
 * degrades the (short) cast to `movsx eax,dx`); all 4 add/mul spelling
 * combos keep `imul edx,eax` (0x3e: target c2 = x-prod stays EAX) plus the
 * coupled w4 EDX/EAX, sar-order bytes (0x46-0x4a,0x4d) and the final d
 * re-read AH/AL pair (0xce,0xd1 -- target `mov ah,[esp]; cmp ah,[ebx+0x1a]`).
 * 9 diff bytes, one allocator 2-cycle. Guards share one return-0 stub.
 *
   0x2e408 -- homing step. Tries up to 4 headings d = dir+step, d-step, ...
 * probing 0x2d468 with the g_dir_dx/g_dir_dy direction step (the 0x2d6c8 idiom).
 * Then if the fresh target angle 0x14c58(dx,dy) is (unsigned) closer than the
 * stored w1e, requires d to be one of the two allowed turns from the launch
 * quadrant p[0x5a] (p[0x5a]-step or p[0x5a]+0x80) else gives up. Decrements
 * the fuel word w42 (0 = give up) and requires turn budget p[0x59] != 0;
 * d == current facing refills p[0x59]=4 else spends one. Commits p[0x1a]=d,
 * returns 1 (0 = remove me). d is a memory-homed char local; i is signed
 * short in EDI (jl). Recipe: -4s -oneatx -zp8 -s -zq
 */
extern short g_dir_dx[];
extern short g_dir_dy[];
extern int FUN_0002d468(int x, int y, int z, unsigned char *obj);
extern short FUN_00014c58(int dx, int dy);

int FUN_0002e408(unsigned char *p, char step)
{
    char d = p[0x1a] + step;
    short i = 0;

    do {
        if ((short)FUN_0002d468(
                (short)((g_dir_dx[(unsigned char)d] * p[0x54] >> 8) + *(short *)(p + 4)),
                (short)(*(short *)(p + 6) + (g_dir_dy[(unsigned char)d] * p[0x54] >> 8)),
                *(short *)(p + 8), p) != 0)
            break;
        d -= step;
        i++;
    } while (i < 4);
    if ((unsigned short)FUN_00014c58(
            (short)(*(short *)(p + 0x2e) - *(short *)(p + 4)),
            (short)(*(short *)(p + 0x30) - *(short *)(p + 6)))
        < *(unsigned short *)(p + 0x1e)) {
        if ((char)(p[0x5a] - step) != d && (char)(p[0x5a] + 0x80) != d)
            goto fail;
    }
    if (--*(unsigned short *)(p + 0x42) == 0 || p[0x59] == 0) {
    fail:
        return 0;
    }
    if (d != p[0x1a])
        p[0x59]--;
    else
        p[0x59] = 4;
    p[0x1a] = d;
    return 1;
}
