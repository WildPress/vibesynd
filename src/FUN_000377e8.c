/* frameless @ 0x377e8: two-level table lookup with a bounds check.
   field = p->[0x44]; node = g_810e + field; if node below g_11670 -> 0;
   else return g_a6ea[node[0x19]] */
extern unsigned char g_810e[];
extern unsigned char g_11670;
extern unsigned char g_a6ea[];
unsigned short FUN_000377e8(unsigned char *p)
{
    unsigned char *node = g_810e + *(unsigned short *)(p + 0x44);
    if (node >= &g_11670)
        return g_a6ea[node[0x19]];
    return 0;
}
