/* Decomp target @ 0x0003c479  (framed wrapper over 0x3c491)
 *
 * NEAR-MISS 21/24 (recipe: -3s -of -s -zq)
 *   target: 5589e5 8b450c 85c0 740c 50 ff7508 e8........ 83c408 5d c3
 *   ours  : 5589e5 837d0c00 740e ff750c ff7508 e8........ 83c408 5d c3
 * Frame, call, arg-a memory push, add-esp and epilogue all identical (call
 * rel32 masked). The only diff: the target loads the guard/arg `b` into EAX
 * once (`mov eax,[ebp+0xc]; test eax,eax; ... push eax`), while our 9.5b keeps
 * `b` in memory for BOTH uses (`cmp [ebp+0xc],0; ... push [ebp+0xc]`). This is
 * the RTL-cluster codegen wall: the shipped 0x3a000+ library materialises the
 * first call argument in EAX; no -3s/-4s opt combo (-oneatx/-of/-ol/-oh/-or)
 * nor a register temp makes our Watcom enregister a param used twice in a
 * function this small. Structure is byte-correct; register-materialisation is
 * not source-reachable. WALL: reg-materialisation / scheduling.
 *
 * => void f(a,b){ if (b) FUN_0003c491(a,b); }
 */
extern void FUN_0003c491(void *a, int b);

void FUN_0003c479(void *a, int b)
{
    if (b)
        FUN_0003c491(a, b);
}
