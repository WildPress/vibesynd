/* frameless @ 0x36c28: walk the chain from p->[0x1c] (base g_810e, link +0x22);
   for each node call FUN_0002fbc8, copy p->[0x16] into node->[0x16], set
   node[0x19]=0x14 and node[0xa] |= 8. */
extern unsigned char g_810e[];
extern void FUN_0002fbc8(unsigned char *node);
void FUN_00036c28(unsigned char *p)
{
    unsigned short id = *(unsigned short *)(p + 0x1c);
    while (id != 0) {
        unsigned char *node = g_810e + id;
        FUN_0002fbc8(node);
        *(unsigned short *)(node + 0x16) = *(unsigned short *)(p + 0x16);
        node[0x19] = 0x14;
        node[0xa] |= 8;
        id = *(unsigned short *)(node + 0x22);
    }
}
