/* FUN_00037658 @ 0x37658 (leaf) - link param_1 into param_2's chain.
 * Chain nodes live in the g_entity_pool pool; next-link is node[0x1c].
 * Returns 0 only when the chain is already full (>6); other paths
 * fall off the end (target: MOV EAX,ESI with ESI unaffected). */
extern unsigned char g_entity_pool[];

int FUN_00037658(int param_1, int param_2)
{
    unsigned short n;
    unsigned char *node;

    n = 0;
    *(short *)(param_1 + 0x1c) = 0;
    if (*(unsigned short *)(param_2 + 0x3a) != 0) {
        node = g_entity_pool + *(unsigned short *)(param_2 + 0x3a);
        while (*(unsigned short *)(node + 0x1c) != 0) {
            node = g_entity_pool + *(unsigned short *)(node + 0x1c);
            n++;
        }
        if (n >= 7)
            return 0;
        *(short *)(param_1 + 0x1e) = (short)(node - g_entity_pool);
        *(unsigned short *)(node + 0x1c) = (short)((unsigned char *)param_1 - g_entity_pool);
    } else {
        *(short *)(param_1 + 0x1e) = (short)((unsigned char *)param_2 - g_entity_pool);
        *(unsigned short *)(param_2 + 0x3a) = (short)((unsigned char *)param_1 - g_entity_pool);
    }
    *(short *)(param_1 + 0x20) = (short)((unsigned char *)param_2 - g_entity_pool);
    *(unsigned char *)(param_1 + 0xa) |= 1;
}
