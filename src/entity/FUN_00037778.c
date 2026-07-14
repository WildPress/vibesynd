/* frameless @ 0x37778: walk the chain from p->[0x3a] (base g_810e, link +0x1c);
   return 1 if any node has byte[0x19] == val, else 0. */
extern unsigned char g_810e[];
int FUN_00037778(unsigned char *p, unsigned short val)
{
    unsigned short id = *(unsigned short *)(p + 0x3a);
    if (id != 0) {
        while (id != 0) {
            unsigned char *node = g_810e + id;
            if (node[0x19] == val)
                return 1;
            id = *(unsigned short *)(node + 0x1c);
        }
    }
    return 0;
}
