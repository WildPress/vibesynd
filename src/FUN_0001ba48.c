/* frameless @ 0x1ba48: clamp a point (param_1,param_2) into the bounding box
   [g_1be32,g_1be36] x [g_1be34,g_1be38], snap to even (& 0xfe), store to g_0000/g_0002,
   reset accumulators g_10ab4/g_10ab8, then draw a 0x19-step diagonal via 25 calls to
   FUN_00045f8a(x+i, y+i, 0x10, g_10ab8, i); finish with FUN_0001a8c8().
   NOTE: manifest size (114B, ends at 0x1baba loop-top) under-counts; true extent is
   0x1ba48-0x1baf2 = 171 bytes. */
extern short g_1be32, g_1be34, g_1be36, g_1be38;
extern int g_10ab4, g_10ab8;
extern unsigned short g_0000, g_0002;
extern int FUN_00045f8a();
extern void FUN_0001a8c8(void);

void FUN_0001ba48(short param_1, short param_2)
{
    unsigned short i;
    g_10ab8 = 0;
    g_10ab4 = 0;
    if (param_1 > g_1be36) param_1 = g_1be36;
    if (param_2 > g_1be38) param_2 = g_1be38;
    if (param_1 < g_1be32) param_1 = g_1be32;
    if (param_2 < g_1be34) param_2 = g_1be34;
    g_0000 = param_1 & 0xfe;
    g_0002 = param_2 & 0xfe;
    for (i = 0; i < 0x19; i++)
        FUN_00045f8a((short)(g_0000 + i), (short)(g_0002 + i), 0x10, g_10ab8, i);
    FUN_0001a8c8();
}
