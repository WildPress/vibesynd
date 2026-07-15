/* @ 0x27e78: far-alloc probe. Alloc a 0x5c far block (DOS int-21h allocator 0x3b239,
   returns seg:off in DX:EAX); if null return -1. Then through the global far pointer
   g_5056 (offset @0x5056, selector @0x505a): stamp byte[0]=0x7f, call 0x27d88 with the
   far ptr; if its (short) result == -1 return -1; finally if byte[1]==3 return 0 else -1.

   NEAR-MISS 89/94 (default -4s -oneatx -zp8 -s -zq). Structure, far-ptr CSE, both other
   `return -1` tails, and the corrected `jnz` branch sense all byte-match target. SOLE diff at
   0x40: the middle `return -1` (call-result == -1 path) -- target re-materialises `mov eax,-1;
   ret` (75 06 b8 ffffffff c3); our Watcom proves EAX already holds -1 (it just compared EAX to
   the immediate -1 and took the equal branch) so it emits a bare `ret` (75 01 c3), 5 bytes
   short. Not source-reachable: any `== -1` compare against the immediate lets Watcom prove
   EAX==-1 on that path and reuse it; a non-immediate errval would change the CMP encoding.
   Block D duplicates correctly only because EAX isn't -1 there. Reverse of the §3 intra-fn
   tail-merge wall (here Watcom reuses/shares, target duplicates) -- block-layout fuzzer may
   close it. -ot/-os diverge far earlier (0x19, they drop the far-ptr CSE). */

extern void __far *FUN_0003b239(unsigned n);
extern short submit_ncb(unsigned char __far *p);

extern unsigned char __far *g_5056;   /* far ptr: off @0x5056, sel @0x505a */

int FUN_00027e78(void)
{
    void __far *blk = FUN_0003b239(0x5c);
    if (blk == 0)
        return -1;
    g_5056[0] = 0x7f;
    if (submit_ncb(g_5056) != -1)
        goto check;
    return -1;
check:
    if (g_5056[1] != 3)
        return -1;
    return 0;
}
