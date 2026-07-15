/* 0x14078 -- net-sync message builder. First time (s[5]==0): send type 4 with
 * the node id (ptr at s+0x10 minus pool base 0x810e), mark s[5]=1. Otherwise
 * probe 0x2e808(s->obj, s->node, (short)(g_a6c2[o[0x19]] * 9 / 10)); on
 * success send types 0x18, 7 (obj w3a), 0x14 (0x7f/0x7f/0xff), and 8 (node
 * coords, z+0x80), each via 0x23158(i); on failure clear s[5] if the obj's
 * type byte is 0. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char g_entity_pool[];
extern unsigned short g_a6c2[];
extern void FUN_00023158(int a);
extern int FUN_0002e808(unsigned char *a, unsigned char *b, int c);

void FUN_00014078(unsigned char *s, unsigned char *m, unsigned char idx,
                  unsigned char *o)
{
    if (s[5] == 0) {
        m[0xd] = 4;
        *(short *)m = (short)(*(unsigned char **)(s + 0x10) - g_entity_pool);
        FUN_00023158((unsigned short)idx);
        s[5] = 1;
        return;
    }
    if (FUN_0002e808(*(unsigned char **)(s + 0xc), *(unsigned char **)(s + 0x10),
                     (short)(g_a6c2[o[0x19]] * 9 / 10)) != 0) {
        unsigned short i = idx;

        m[0xd] = 0x18;
        FUN_00023158(i);
        m[0xd] = 7;
        *(short *)m = *(short *)(*(unsigned char **)(s + 0xc) + 0x3a);
        FUN_00023158(i);
        m[0xd] = 0x14;
        *(short *)m = 0x7f;
        *(short *)(m + 2) = 0x7f;
        *(short *)(m + 4) = 0xff;
        FUN_00023158(i);
        m[0xd] = 8;
        *(short *)m = *(short *)(*(unsigned char **)(s + 0x10) + 4);
        *(short *)(m + 2) = *(short *)(*(unsigned char **)(s + 0x10) + 6);
        *(short *)(m + 4) = *(short *)(*(unsigned char **)(s + 0x10) + 8) + 0x80;
        FUN_00023158(i);
    } else {
        if (*(*(unsigned char **)(s + 0xc) + 0x19) == 0)
            s[5] = 0;
    }
}
