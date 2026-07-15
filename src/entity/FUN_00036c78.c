/* FUN_00036c78 @ 0x36c78 - walk pool chain (g_entity_pool) from node[0x1c], reset nodes */
extern unsigned char g_entity_pool[];
extern void detach_entity_type(void *node);

void FUN_00036c78(int param_1)
{
    unsigned short id;
    unsigned short val;
    unsigned char *node;

    if (*(unsigned char *)(param_1 + 0x19) != 5 &&
        *(unsigned char *)(param_1 + 0x19) != 6) {
        id = *(unsigned short *)(param_1 + 0x1c);
        if (id != 0) {
            do {
                node = g_entity_pool + id;
                if (*(short *)(node + 0x20) == 0) {
                    if (node[0x1c] & 1) {
                        detach_entity_type(node);
                        node[0x19] = 0x1f;
                        node[0x58] = 0x1f;
                        val = *(unsigned short *)(param_1 + 0x16);
                        node[0x4a] = 0xff;
                        node[0x49] = 0xff;
                        node[0x4e] = 0xff;
                        node[0x4d] = 0xff;
                        node[0x52] = 0xff;
                        node[0x51] = 0xff;
                        *(unsigned short *)(node + 0x2a) = val;
                    } else if (node[0x1c] & 0xc) {
                        detach_entity_type(node);
                        node[0x19] = 0x20;
                        node[0x58] = 0x20;
                        val = *(unsigned short *)(param_1 + 0x16);
                        node[0x4a] = 0xff;
                        node[0x49] = 0xff;
                        node[0x4e] = 0xff;
                        node[0x4d] = 0xff;
                        node[0x52] = 0xff;
                        node[0x51] = 0xff;
                        *(unsigned short *)(node + 0x2a) = val;
                    }
                }
                id = *(unsigned short *)(node + 0x22);
            } while (id != 0);
        }
    }
}
