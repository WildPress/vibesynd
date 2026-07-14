/* PARKED near-miss (NOT matched) @ 0x1b858 (165B) -- draw-list walk over the g_5340 pool.
   Sibling of 0x1b798. Logic verified instruction-exact (the whole FUN_0004a63a call region
   is byte-identical masked). Plain 3-arg STACK-call fn (-4s); NOT __thiscall (the decompiler
   was wrong -- params come from [ESP+..], and -4r diverges at byte 0).
   WALL (register-allocation/CSE, best 121/165, 5000 cpermute variants): the target reloads
   g_5314 each iteration into callee-saved EBP (4 pushes EBX/ESI/EDI/EBP) and reloads g_5340
   uncached; our -oneatx build (relaxed alias) uses EDX (3 pushes) and CSE-merges the global
   loads, so the arg stack offsets shift by +4. Not reachable from C under the required recipe.
   Each record is 5 ushorts [val,x,y,arg,next]; emit FUN_0004a63a for nodes with val+g_5314>g_5314. */
extern unsigned char  *g_5338;
extern unsigned short *g_5340;
extern unsigned int    g_5314;
extern void FUN_0004a63a(int, int, unsigned int, unsigned int);

void FUN_0001b858(unsigned short param_1, short param_2, short param_3)
{
    unsigned short *p;

    p = g_5340 + (unsigned)*(unsigned short *)(g_5338 + (unsigned)param_1 * 8) * 5;
    if (g_5340 < p)
    do {
        if (*p + g_5314 > g_5314)
            FUN_0004a63a((short)(p[1] + param_2), (short)(p[2] + param_3),
                         *p + g_5314, p[3]);
        p = g_5340 + (unsigned)p[4] * 5;
    } while (g_5340 < p);
}
