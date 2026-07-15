/* FUN_000223c8 @ 0x223c8 (724B true size; manifest 686 excludes the 38B of
 * jump-table case bodies hidden behind the CS:[eax*4+0x14c70] dispatch).
 * PARKED ~694/724 (-4s -oneatx -zp8 -s -zq; verify with truediff.py 724):
 * everything matches except the TWO node-pointer formations, where 9.5b
 * emits the 1-byte-shorter `imul ebp,edx,0x5c; add ebp,imm32(g_pool_a)` while
 * the target has `imul edx,edx,0x5c; mov ebp,0x8110; add ebp,edx` (plus the
 * jz/jnz/jc disp bytes cascaded from the 2B length delta). Tried: plain,
 * (int)-cast base, named int prod, named ushort idx, two-statement init --
 * all inert or regressive. Same codegen-choice family as the scaled-index
 * lea materialization wall. LEVER NOTES: the `*(short*)(node+0xa) &= ~0x109`
 * COMPOUND spelling is load-bearing three ways (imm32 AND on DI, sunk store,
 * AND it flips the d-loop j/d ESI-EBX roles); the named ushort t carrying
 * the +0x6b template word restores site-1's EBX-first row allocation that
 * the compound form otherwise steals; the template loads after the chain
 * walk then materialize `lea eax,[eax*8]; add eax,ebx` instead of folding
 * scale-8 into the SIB.
 * Squad/row slot re-equip from the per-row template
 * table at 0xe551 (stride 1047 bytes/row): row header byte +0 = base slot
 * index into pool A (0x8110, 0x5c-stride records); 18 template entries of
 * 40B each at row+0x69: {word ->node+0x14, word ->node+0x3c, byte 1-based
 * slot no, 8 x (short count, ushort kind) pairs at +7}. For each present
 * entry: free the node's carried-item chain (FUN_269b8 each, link +0x1c,
 * base g_entity_pool), reset node fields (+0x3a=0, +0x3c/+0x14 from template,
 * +0x1c = mode|0x400, +0xa &= ~0x109, +0x19=0, +0xc=0), set size bytes
 * +0x55/+0x56 = 0x28/0x30/0x38/0x40 by (node[0x3c]&6)>>1 (4-entry jump
 * table at 0x14c70), then create up to 8 items FUN_226a8(node, kind, count)
 * storing the results in g_dcbc[j][d] (dword [j*0x20 + d*4]). Finally,
 * unless mode == 0x1002, every one of the 8 slots NOT touched above is
 * fully freed (chain + node) and zeroed (FUN_4d199(node,0,0x5c)). */
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char g_dcbc[];
extern unsigned char g_e551[];
extern void FUN_000269b8(unsigned char *p);
extern int FUN_000226a8(unsigned char *node, unsigned short kind, int cnt);
extern void FUN_0004d199(unsigned char *p, int val, int n);

void FUN_000223c8(unsigned short row, unsigned short mode)
{
    unsigned short used[8];
    unsigned short j;
    unsigned short i;
    unsigned short s;
    unsigned short d;
    unsigned short id;
    unsigned char *node;
    unsigned char *p;
    unsigned short t;

    for (i = 0; i < 8; i++)
        used[i] = 0;
    for (s = 0; s < 4; s++)
        for (i = 0; i < 8; i++)
            *(int *)(g_dcbc + s * 0x20 + i * 4) = 0;
    for (j = 0; j < 0x12; j++) {
        if (*(unsigned char *)(g_e551 + row * 1047 + j * 40 + 0x6f) != 0) {
            node = g_pool_a + (*(unsigned char *)(g_e551 + row * 1047) +
                             *(unsigned char *)(g_e551 + row * 1047 + j * 40 + 0x6f) - 1) * 0x5c;
            used[*(unsigned char *)(g_e551 + row * 1047 + j * 40 + 0x6f) - 1] = 1;
            id = *(unsigned short *)(node + 0x3a);
            while (id != 0) {
                p = g_entity_pool + id;
                id = *(unsigned short *)(p + 0x1c);
                FUN_000269b8(p);
            }
            *(unsigned short *)(node + 0x3a) = 0;
            t = *(unsigned short *)(g_e551 + row * 1047 + j * 40 + 0x6b);
            *(unsigned short *)(node + 0x3c) = t;
            *(unsigned short *)(node + 0x14) = *(unsigned short *)(g_e551 + row * 1047 + j * 40 + 0x69);
            *(unsigned short *)(node + 0x1c) = mode | 0x400;
            *(short *)(node + 0xa) &= ~0x109;
            node[0x19] = 0;
            *(unsigned short *)(node + 0xc) = 0;
            switch ((unsigned short)(*(unsigned short *)(node + 0x3c) & 6) >> 1) {
            case 0:
                node[0x56] = 0x28;
                node[0x55] = 0x28;
                break;
            case 1:
                node[0x56] = 0x30;
                node[0x55] = 0x30;
                break;
            case 2:
                node[0x56] = 0x38;
                node[0x55] = 0x38;
                break;
            case 3:
                node[0x56] = 0x40;
                node[0x55] = 0x40;
                break;
            }
            for (d = 0; d < 8; d++) {
                if (*(unsigned short *)(g_e551 + row * 1047 + j * 40 + d * 4 + 0x72) > 0) {
                    *(int *)(g_dcbc + j * 0x20 + d * 4) =
                        FUN_000226a8(node,
                                     *(unsigned short *)(g_e551 + row * 1047 + j * 40 + d * 4 + 0x72),
                                     *(short *)(g_e551 + row * 1047 + j * 40 + d * 4 + 0x70));
                }
            }
        }
    }
    if (mode != 0x1002) {
        for (d = 0; d < 8; d++) {
            if (used[d] == 0) {
                node = g_pool_a + (d + *(unsigned char *)(g_e551 + row * 1047)) * 0x5c;
                id = *(unsigned short *)(node + 0x3a);
                while (id != 0) {
                    p = g_entity_pool + id;
                    id = *(unsigned short *)(p + 0x1c);
                    FUN_000269b8(p);
                }
                FUN_000269b8(node);
                FUN_0004d199(node, 0, 0x5c);
            }
        }
    }
}
