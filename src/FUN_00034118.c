/* frameless @ 0x34118: node = g_810e + p2->[0x1c]; if node[0x54] > val store
   node[0x54]-val into dest[0x54], else store 0. */
extern unsigned char g_810e[];
void FUN_00034118(unsigned char *dest, unsigned char *p2, unsigned short val)
{
    unsigned short id = *(unsigned short *)(p2 + 0x1c);
    if (id != 0) {
        unsigned char *node = g_810e + id;
        if (node[0x54] > val) {
            dest[0x54] = node[0x54] - val;
            return;
        }
    }
    dest[0x54] = 0;
}
