/* frameless @ 0x376f8: walk the chain from p->[0x3a] (base g_810e, link +0x1c)
   and sum a 16-bit table value g_a7b4[node[0x19]] over every node. */
extern unsigned char g_810e[];
extern unsigned short g_a7b4[];
int FUN_000376f8(unsigned char *p)
{
    unsigned short id = *(unsigned short *)(p + 0x3a);
    unsigned short sum = 0;
    if (id != 0) {
        while (id != 0) {
            unsigned char *node = g_810e + id;
            sum += g_a7b4[node[0x19]];
            id = *(unsigned short *)(node + 0x1c);
        }
    }
    return sum;
}
