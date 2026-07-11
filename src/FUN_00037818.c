/* frameless @ 0x37818: pool accessor. node = g_810e + p[0x44] (u16 id); if the node
   address is within the valid pool region (>= g_810e + 0x9562) return the byte table
   g_a686 indexed by node[0x19], else 0. Sibling of the matched 0x37738.

   PARKED near-miss (NOT matched). Logic correct, but the target's `return 0` jumps
   BACKWARD to 0x3780f -- the `xor eax,eax; ret` tail of the PREVIOUS function
   FUN_000377e8 -- i.e. Watcom tail-merged the return-0 stub across the two sibling
   accessors that shared a source module. Compiling this fn in isolation always emits
   a LOCAL return-0 (40B vs target 39B). Only reachable by compiling it together with
   0x377e8 in one file at the exact inter-fn padding; not worth it for one 39B fn.
   (The other 4 siblings 0x37738/78/b8/e8 matched individually -- they don't reuse a
   neighbour's stub.) */
extern unsigned char g_810e[];
extern unsigned char g_a686[];
unsigned char FUN_00037818(unsigned char *p)
{
    unsigned char *node = g_810e + *(unsigned short *)(p + 0x44);
    if (node >= g_810e + 0x9562)
        return g_a686[node[0x19]];
    return 0;
}
