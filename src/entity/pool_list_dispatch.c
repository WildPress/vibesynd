/* frameless @ 0x2e588: dispatch based on a pool linked list hung off param_2[0x24].
   If param_2[0x19]==0x17 return 0. If the list is empty, tail to los_trace. Else
   walk node[0x24] "next" links (node = g_entity_pool + id) until a node with node[0x18]==2 or
   the list ends; call los_trace_far(param_1, node, param_3); return param_2 if it echoed
   node back, else 0.

   MATCHED (RELOC-AWARE, masked): YES. EDIT-DIST 49 -> 5; the residual 5 bytes are all
   masked relocations (g_entity_pool base in `add ebx,0x810e`, and the two call rel32s).
   FIX was pure block ordering, NOT a register wall. The old note blamed a param_1/param_3
   EDX<->ECX swap and parked it; wrong diagnosis. The target lays the loop/los_trace_far
   path as the fall-through body and the `id==0` los_trace call as the tail, with the
   shared `return 0` hoisted to the top (fall-through of the ==0x17 guard). Rewriting
   `if(id==0) return los_trace(...); while(...)` as
   `if(id!=0){ while(...); ...los_trace_far... } return los_trace(...)` reorders the blocks
   to match AND fixes the register allocation for free (param_1->EDX, param_3->ECX fall out
   of the new schedule). Full match. */
extern unsigned char g_entity_pool[];
extern unsigned short *los_trace(unsigned short *param_1, unsigned short *param_2, int c);
extern unsigned char *los_trace_far(unsigned short *param_1, unsigned char *node, int c);
unsigned short *pool_list_dispatch(unsigned short *param_1, unsigned short *param_2, int param_3)
{
    unsigned short id;
    unsigned char *node;
    if (*((unsigned char *)param_2 + 0x19) == 0x17)
        return 0;
    id = *(unsigned short *)((unsigned char *)param_2 + 0x24);
    if (id != 0) {
        while (id != 0) {
            node = g_entity_pool + id;
            if (node[0x18] == 2)
                break;
            id = *(unsigned short *)(node + 0x24);
        }
        if (los_trace_far(param_1, node, (short)param_3) == node)
            return param_2;
        return 0;
    }
    return los_trace(param_1, param_2, (short)param_3);
}
