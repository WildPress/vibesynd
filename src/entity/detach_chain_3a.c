/* frameless @ 0x37a48: detach the chain at p->[0x3a] then walk the old chain
   (base g_entity_pool, link +0x1c) calling grid_detach_object on every node. */
extern unsigned char g_entity_pool[];
extern void grid_detach_object(unsigned char *node);
void detach_chain_3a(unsigned char *p)
{
    unsigned short id = *(unsigned short *)(p + 0x3a);
    if (id != 0) {
        *(unsigned short *)(p + 0x3a) = 0;
        p[0x46] = 0;
        *(unsigned short *)(p + 0x44) = 0;
        while (id != 0) {
            unsigned char *node = g_entity_pool + id;
            grid_detach_object(node);
            id = *(unsigned short *)(node + 0x1c);
        }
    }
}
