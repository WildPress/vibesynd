/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): load-scheduling + load-width tie.
   Full 34B/33B byte walk: only diffs are (a) target loads p2 before p1, ours after, and
   (b) target loads p2 16-bit (mov ax,[esp+8]) vs ours 32-bit (mov eax,[esp+8]) -- both
   masked by the same `and eax,0xffff` before push, so identical value. Same conditions
   (test edx,edx/jne; test ax,ax/je), same call target FUN_000287c8, same arg order
   (push eax=p2, push edx=p1), same epilogue. Comment-only; output byte-identical.
   frameless @ 0x27ed8: if(p1||p2) FUN_000287c8(p1,(unsigned short)p2). NEAR-MISS (parked).
   Logic exact but the target loads p2 (16-bit `mov ax,[esp+8]`) BEFORE p1 and no source form
   flips that scheduling (first diff at 0x0, 66 vs 8b). Instruction-scheduling wall. */
extern void FUN_000287c8(int a, int b);
void guarded_call_2arg(int p1, unsigned short p2)
{
    if (p1 != 0 || p2 != 0)
        FUN_000287c8(p1, p2);
}
