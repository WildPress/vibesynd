/* C runtime: fgetc @ 0x3d3e4 (CLIB3S). NEAR-MISS (parked). Logic is faithful (refill via
   FUN_0003d40f, then cnt--/ptr++/return ptr[-1]) but three codegen diffs block it:
   (1) the `return -1` path duplicates the epilogue where the target shares it (jmp eb0d) --
   the intra-function tail-merge wall; (2) Watcom schedules the dec/inc/load in a different
   order; (3) ours emits one-step `movzx eax,[eax-1]` vs the target's two-step `mov al;movzx`.
   None source-steerable under -3s -d2 -oneatx (playbook S3). */
extern int FUN_0003d40f(unsigned char *fp);
int FUN_0003d3e4(unsigned char *fp)
{
    if (FUN_0003d40f(fp) == 0)
        return -1;
    *(int *)(fp + 4) -= 1;
    *(unsigned char **)fp += 1;
    return (*(unsigned char **)fp)[-1];
}
