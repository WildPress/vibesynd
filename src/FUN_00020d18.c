/* frameless @ 0x20d18: relocate the map column-offset table. The caller passes a
   block base; the column table starts at base+0xc and holds 0x3000 int entries that
   are stored on disk as relative offsets. Walk it, adding the table base to each
   entry to turn it into an absolute pointer, then publish the base in g_5358.

   WALL (52B target vs 43B ours; logic byte-exact). The target uses a LESS-folded /
   loop-aligned codegen our Watcom 9.5b never emits at ANY recipe: it holds the base in
   a callee-saved register (`push esi` .. `esi=base`), materialises the element address
   once in EAX (`movsx; lea eax,[eax*4]; add eax,esi`) and shares it between the load and
   store (`mov ecx,[eax]; lea edx,[esi+ecx]; mov [eax],edx`), and pads the loop head with
   alignment NOPs (`8d4000; 8bc9`). Every -o* variant (-oneatx/-ot/-oi/-oat/-oax/-ox/-ol/
   -oal/-or/-os/-oe...) folds the addressing into modrm `[edx+eax*4]` with the base in a
   scratch reg, no ESI save, no alignment pad -> a tighter 43B form. This is the loop-align
   (§3) + un-folded/callee-saved-base register wall combined; not source-reachable. Any
   `p[i]+=(int)p` / `q=p+i; *q+=(int)p` / `int base; *q+=base` spelling yields the same 43B. */
extern int *g_5358;
void FUN_00020d18(int param_1)
{
    int *p = (int *)(param_1 + 0xc);
    short i = 0;
    do {
        p[i] += (int)p;
        i++;
    } while (i < 0x3000);
    g_5358 = p;
}
