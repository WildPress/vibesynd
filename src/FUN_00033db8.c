/* PARKED near-miss (NOT matched, 154/167) -- spatial-grid proximity scan. Structure 100%
   correct (offsets, signedness, -0x80 check, JBE health test, CMP DI,6, return width). Two
   §3 ties remain: (1) prologue param-load/init scheduling permutation ~10B at 0x04; (2) the
   grid-idx sign-extend `CWDE; MOV EDX,EAX` (target) vs `MOVSX EDX,AX` (ours), same encoding-tie
   class as 0x34088. Near-twins 0x33b88/0x33c38/0x33cf8 share this template and this wall. */
/* frameless @ 0x33db8: spatial-grid proximity scan. Walk 6 grid rows starting at
   (param_2 - 0x100), stepping +0x100 per row. For each row build the 128x128
   grid-cell index from the high bits of iVar3 (>>1) and of param_1 (>>8), and walk
   the object chain rooted at g_10e[idx] (node = g_810e + id, next-link at node+0).
   Return 1 for the first node that is type 2 (node[0x18]==2), on the same level as
   param_3 (high byte of node[8] == high byte of param_3), has a secondary link
   node[0x1c]!=0, faces 0x80 (node[0x1a]==0x80), and whose linked node has a nonzero
   value byte at +0x54. Return 0 after 6 rows with no hit. Near-twin of FUN_00033b88. */
extern unsigned char g_810e[];
extern unsigned short g_10e[];

int FUN_00033db8(short param_1, int param_2, short param_3)
{
    int iVar3;
    unsigned short row;
    for (iVar3 = param_2 - 0x100, row = 0; row < 6; iVar3 += 0x100, row++) {
        unsigned short head =
            g_10e[((short)(iVar3 & 0x7f00) >> 1) | ((param_1 >> 8) & 0x7f)];
        while (head != 0) {
            unsigned char *node = g_810e + head;
            if (node[0x18] == 2 &&
                (0xff00 & *(short *)(node + 8)) == (param_3 & 0xff00) &&
                *(unsigned short *)(node + 0x1c) != 0 &&
                node[0x1a] == 0x80) {
                unsigned char *n2 = g_810e + *(unsigned short *)(node + 0x1c);
                if (n2[0x54] > 0)
                    return 1;
            }
            head = *(unsigned short *)node;
        }
    }
    return 0;
}
