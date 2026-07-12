/* Decomp target @ 0x0003c42d  (framed, non-leaf; uses EBX)
 *
 * NEAR-MISS 28/32 (recipe: -3s -of -s -zq  -- NB: NO -oneatx; -oneatx rotates
 * the loop into a do-while + pre-increment and breaks the body match)
 *   target: 53 5589e5 8b5d0c  803b00 7411 0fb603 50 e8........ 83c404 8803 43 ebea 5d5b c3
 *   ours  : 5589e5 53 8b5d08  803b00 7411 0fb603 50 e8........ 83c404 8803 43 ebea 5b5d c3
 * The ENTIRE loop body is byte-identical (call rel32 masked): top-tested
 * `while(*s)` with `movzx eax,[ebx]; call 0x3dce5; mov [ebx],al; inc ebx; jmp`.
 * The only diff is the prologue register-save ORDER: the target saves EBX
 * BEFORE establishing the frame (`push ebx; push ebp; mov ebp,esp`), which puts
 * param `s` at [ebp+0xc] and epilogue `pop ebp; pop ebx`; our 9.5b saves EBX
 * AFTER the frame (`push ebp; mov ebp,esp; push ebx`) -> [ebp+8] and `pop ebx;
 * pop ebp`. Same bytes, swapped order. Every -of variant (-of/-of+, -3s/-4s/
 * -5s, -zp1/8, -oneatx/-ol/-oh/-or/-ot, plain) emits the ebp-first prologue;
 * dropping -of removes the frame entirely (params via [esp+8], no ebp). The
 * ebx-outermost save order is not reachable from our compiler/flags. WALL:
 * prologue register-save order (RTL cluster).
 *
 * => in-place transform of a NUL-terminated byte string (tolower/toupper-like).
 */
extern unsigned char FUN_0003dce5(unsigned char c);

void FUN_0003c42d(unsigned char *s)
{
    while (*s) {
        *s = FUN_0003dce5(*s);
        s++;
    }
}
