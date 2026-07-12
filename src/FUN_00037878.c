/* FUN_00037878 @ 0x37878 (leaf) - unlink param_1 from its pool chain.
 * Chain nodes live in the g_810e pool; node = g_810e + param_1[0x44].
 * prev id = node[0x1e], next id = node[0x1c], scratch at node[0x20],
 * flag byte node[0xa]. Two cases: param_1 is the chain head (its own
 * prev-slot points back at it) vs mid-chain. Sibling: FUN_00037658. */
extern unsigned char g_810e[];

void FUN_00037878(int param_1)
{
    unsigned char *node;
    unsigned char *prev;
    unsigned char *nextnode;

    if (*(unsigned short *)(param_1 + 0x44) != 0) {
        node = g_810e + *(unsigned short *)(param_1 + 0x44);
        prev = g_810e + *(unsigned short *)(node + 0x1e);
        if ((unsigned char *)param_1 == prev) {
            if (*(unsigned short *)(node + 0x1c) != 0) {
                nextnode = g_810e + *(unsigned short *)(node + 0x1c);
                *(short *)(nextnode + 0x1e) = *(short *)(node + 0x1e);
            }
            *(short *)(param_1 + 0x3a) = *(short *)(node + 0x1c);
        } else {
            *(short *)(prev + 0x1c) = *(short *)(node + 0x1c);
            if (*(unsigned short *)(node + 0x1c) != 0) {
                nextnode = g_810e + *(unsigned short *)(node + 0x1c);
                *(short *)(nextnode + 0x1e) = *(short *)(node + 0x1e);
            }
        }
        *(short *)(node + 0x1e) = 0;
        *(short *)(node + 0x1c) = 0;
        *(short *)(node + 0x20) = 0;
        *(unsigned char *)(node + 0xa) &= 0xfe;
    }
    *(unsigned char *)(param_1 + 0x46) = 0;
    *(short *)(param_1 + 0x44) = 0;
}
