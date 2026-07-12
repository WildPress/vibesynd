/* framed @ 0x3d3e4: getc-style byte fetch from a buffered stream. Calls the
   refill/check helper FUN_0003d40f(p); on 0 returns -1, else decrements the
   count at p[1], advances the buffer pointer p[0], and returns the byte it
   stepped over (zero-extended). param arrives at [EBP+0xc] (EBX saved first).

   NEAR-MISS 41/43 bytes, recipe `-3s -os -ol -zp8 -s -zq`. Body is byte-perfect
   (EAX reload + two-step `mov al,[eax-1]; movzx eax,al` come from `-ol` + the
   char temp `c`). Sole remaining diff: the -1 branch. Target shares one epilogue
   (`mov eax,-1; jmp END` -> single `pop ebp;pop ebx;ret`); Watcom 9.5b instead
   duplicates the 3-byte `pop;pop;ret` tail (jnz disp 07->08, then eb0d->5d5bc3,
   ours 1 byte longer). Shared-epilogue / tail-merge WALL (playbook s3) -- not
   reachable from source: early-return, single-exit int r, and goto-out all
   duplicate; no flag (-os/-ol/-oa/-ot/-oc) makes 9.5b cross-jump this tail. */
struct stream { unsigned char *ptr; int cnt; };
extern int FUN_0003d40f(struct stream *);

int FUN_0003d3e4(struct stream *p)
{
    unsigned char c;
    if (FUN_0003d40f(p) == 0)
        return -1;
    p->cnt--;
    p->ptr++;
    c = p->ptr[-1];
    return c;
}
