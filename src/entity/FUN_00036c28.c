/* frameless @ 0x36c28: walk the chain from p->[0x1c] (base g_entity_pool, link +0x22);
   for each node call detach_entity_type, copy p->[0x16] into node->[0x16], set
   node[0x19]=0x14 and node[0xa] |= 8. */
extern unsigned char g_entity_pool[];
extern void detach_entity_type(unsigned char *node);
void FUN_00036c28(unsigned char *p)
{
    unsigned short id = *(unsigned short *)(p + 0x1c);
    while (id != 0) {
        unsigned char *node = g_entity_pool + id;
        detach_entity_type(node);
        *(unsigned short *)(node + 0x16) = *(unsigned short *)(p + 0x16);
        node[0x19] = 0x14;
        node[0xa] |= 8;
        id = *(unsigned short *)(node + 0x22);
    }
}
