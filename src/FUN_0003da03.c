/* C runtime: ftell @ 0x3da03 (CLIB3S). NEAR-MISS (parked). Register allocation is byte-exact
   (fp->ebx, r->edx); the ONLY diff is a 2-byte intra-function tail-merge: the target shares one
   `mov eax,edx`+epilogue (via jmp eb03) between the add/sub branches, ours duplicates it. No -o
   flag (-os/-oe/-ol/...) shares the tail. Tail-merge wall (playbook S3). Callee is tell 0x3a97c. */
extern long FUN_0003a97c(int fd);
long FUN_0003da03(unsigned char *fp)
{
    long r = FUN_0003a97c(*(int *)(fp + 0x10));
    long pos = r;
    if (r != -1) {
        if (*(int *)(fp + 4) != 0) {
            if (fp[0xd] & 0x10)
                pos = *(int *)(fp + 4) + r;
            else
                pos = r - *(int *)(fp + 4);
        }
    }
    return pos;
}
