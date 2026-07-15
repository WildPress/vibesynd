/* frameless @ 0x2e588: dispatch based on a pool linked list hung off param_2[0x24].
   If param_2[0x19]==0x17 return 0. If the list is empty, tail to los_trace. Else
   walk node[0x24] "next" links (node = g_entity_pool + id) until a node with node[0x18]==2 or
   the list ends; call los_trace_far(param_1, node, param_3); return param_2 if it echoed
   node back, else 0.

   PARKED near-miss (NOT matched; logic correct). Register-allocation wall: the target
   puts param_1 in EDX and param_3 in ECX; every C spelling swaps them (param_1->ECX,
   param_3->EDX), and it also lays the first `return 0` inline (fall-through) where we
   merge both return-0 paths to the end. cpermute plateaus at 57/102 -- the swap cascades
   through the whole body. Not source-reachable. */
extern unsigned char g_entity_pool[];
extern unsigned short *los_trace(unsigned short *param_1, unsigned short *param_2, int c);
extern unsigned char *los_trace_far(unsigned short *param_1, unsigned char *node, int c);
unsigned short *FUN_0002e588(unsigned short *param_1, unsigned short *param_2, int param_3)
{
    unsigned short id;
    unsigned char *node;
    if (*((unsigned char *)param_2 + 0x19) == 0x17)
        return 0;
    id = *(unsigned short *)((unsigned char *)param_2 + 0x24);
    if (id == 0)
        return los_trace(param_1, param_2, (short)param_3);
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
