/* frameless @ 0x13bc8: walk a pool linked list to its terminal node. Start from the
   node id at (*(int*)(param_5+0xc))[0x3a]; node = g_entity_pool + id. Unless the first node
   already has node[0x19]==1, follow the node[0x1c] "next" links while next != 0 and
   node[0x19] != 1. Store the final node's id (node - g_entity_pool) into *obj1, then call
   run_mission_command(param_4). obj1[0xd] is set to 7 and param_5[5] cleared up front. */
extern unsigned char g_entity_pool[];
extern void run_mission_command(unsigned int);
void pool_list_last(short *obj1, int p2, int p3, unsigned char param_4, unsigned char *param_5)
{
    unsigned char *node;
    param_5[5] = 0;
    *((unsigned char *)obj1 + 0xd) = 7;
    node = g_entity_pool + *(unsigned short *)(*(int *)(param_5 + 0xc) + 0x3a);
    if (node[0x19] != 1) {
        while (*(unsigned short *)(node + 0x1c) != 0 && node[0x19] != 1)
            node = g_entity_pool + *(unsigned short *)(node + 0x1c);
    }
    *obj1 = (unsigned short)(node - g_entity_pool);
    run_mission_command((unsigned short)param_4);
}
