/* frameless @ 0x377b8: count the length of a linked chain
   starting from a 16-bit id in p->[0x3a], following link at node+0x1c */
extern unsigned char g_810e[];
int FUN_000377b8(unsigned char *p)
{
    unsigned short id = *(unsigned short *)(p + 0x3a);
    int count = 0;
    if (id != 0) {
        while (id != 0) {
            unsigned char *node = g_810e + id;
            count++;
            id = *(unsigned short *)(node + 0x1c);
        }
    }
    return count;
}
