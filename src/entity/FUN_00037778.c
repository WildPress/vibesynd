/* frameless @ 0x37778: walk the chain from p->[0x3a] (base g_entity_pool, link +0x1c);
   return 1 if any node has byte[0x19] == val, else 0. */
extern unsigned char g_entity_pool[];
int FUN_00037778(unsigned char *p, unsigned short val)
{
    unsigned short id = *(unsigned short *)(p + 0x3a);
    if (id != 0) {
        while (id != 0) {
            unsigned char *node = g_entity_pool + id;
            if (node[0x19] == val)
                return 1;
            id = *(unsigned short *)(node + 0x1c);
        }
    }
    return 0;
}
