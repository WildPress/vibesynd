/* Decomp target @ 0x0003c491  (framed, non-leaf; callee of wrapper 0x3c479)
 *
 * NEAR-MISS 21/23 (recipe: -3s -of -s -zq)
 *   target: 5589e5 8b4508 837d0c00 7409 50   e8........ 83c404 5d c3
 *   ours  : 5589e5          837d0c00 740b ff7508 e8........ 83c404 5d c3
 * Frame, guard `cmp [ebp+0xc],0`, call, add-esp and epilogue identical (call
 * rel32 masked). Only diff: the target loads arg `a` into EAX before the guard
 * and pushes the register (`mov eax,[ebp+8]; ... push eax`), whereas our 9.5b
 * pushes the memory operand directly (`push [ebp+8]`). Same RTL-cluster wall as
 * 0x3c479 / 0x3c42d: the shipped 0x3a000+ library materialises the first call
 * argument in EAX. Tried -oneatx/-of/-oh/-ol/-or/-ox/-oe/-om, plain -3s, -4s,
 * and a local copy of `a`; every combo emits the memory push. Not source-
 * reachable. WALL: reg-materialisation / scheduling.
 *
 * => void f(a,b){ if (b) FUN_0003c4b9(a); }
 */
extern void FUN_0003c4b9(void *a);

void FUN_0003c491(void *a, int b)
{
    if (b)
        FUN_0003c4b9(a);
}
