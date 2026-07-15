/* frameless @ 0x20d18: relocate the map column-offset table. The caller passes a
   block base; the column table starts at base+0xc and holds 0x3000 int entries that
   are stored on disk as relative offsets. Walk it, adding the table base to each
   entry to turn it into an absolute pointer, then publish the base in g_map_cols.

   PARKED WALL, improved by the cont.21 retry: 43B -> 45B vs the 52B target, and the
   old "loop-align + callee-saved-base" bundle is now mostly RECOVERED. The winning
   lever was the whole-index NAMED INT temp + named char* value local (`int addr =
   i*4 + (int)base; char *v = *(char **)addr; *(char **)addr = base + (int)v;`):
   this alone brings in the `push esi` callee-save, the pointer-add `lea` for the
   sum, and -- confirming the old note -- the loop-head alignment pads appear
   AUTOMATICALLY under -oneatx and are byte-identical to the target (`8d4000; 8bc9`).
   Padding is NOT the blocker. TWO residues remain:
   (1) base ESI<->EDX role swap (ours: base=EDX/sum=ESI via `lea esi,[edx+ecx]`;
       target: base=ESI/sum=EDX via `lea edx,[esi+ecx]`) -- addend-position swap on
       addr is byte-inert; register-role tie-break family;
   (2) the 7-byte fold gap: ours still folds both accesses into modrm `[edx+eax*4]`
       where the target materialises the slot address once in EAX
       (`lea eax,[eax*4+0]; add eax,esi; mov ecx,[eax]; ...; mov [eax],edx`).
       The named-addr, char** slot + (int)*slot (0x2d5b8 idiom), named-base,
       pointer-variable g_map_cols decl, and product-first spellings all still fold;
       `volatile` on the deref REGRESSES to a 38B RMW `add [edx+eax*4],edx`;
       -ot / no-opt recipes drop the pads and still fold. Un-folded-addressing
       codegen our 9.5b does not emit -- same conclusion as the original park,
       but now isolated to the fold + one role swap. */
extern char **g_map_cols;
void FUN_00020d18(int param_1)
{
    char *base = (char *)param_1 + 0xc;
    short i = 0;
    do {
        int addr = i * 4 + (int)base;
        char *v = *(char **)addr;
        *(char **)addr = base + (int)v;
        i++;
    } while (i < 0x3000);
    g_map_cols = (char **)base;
}
