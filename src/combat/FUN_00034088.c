/* frameless @ 0x34088: collision query along a shot path. The shot's accumulated
   position lives in the globals g_10b5e (x), g_10b5c (y), g_10b5a (level). Build the
   128x128 spatial-grid cell index from the high bytes of x,y and walk the object
   linked list rooted at g_10e[idx]. Return the first object (node = g_810e + id) that
   is not flagged (byte[0xb]&1, byte[0xa]&1 both clear), sits on the same level (high
   byte of node[8] == high byte of g_10b5a), and is of type 1 or 2. NULL if none.
   g_10b5a is read every iteration, so -oa homes it in CX and writes it back at exit.

   PARKED near-miss (128/129, NOT matched -> not in recipes/manifest). Everything
   matches -- the idx build, the CX cache+writeback, the flag/level/type checks, the
   register layout of the level compare (node8 in EBX, level in EDX) -- except ONE
   byte: the CMP modrm. The target encodes CMP EDX,EBX (reg field = node8, the
   memory operand, 39 da); Watcom gives us CMP EBX,EDX (reg field = the second
   operand, 39 d3) for every C spelling. Confirmed: both operand orders and a node8
   temp all reproduce the same d3, and 8000 cpermute variants plateau at 128/129. A
   pure compare-encoding tie-break, same class as 0x34048 / 0x26e18. */
extern unsigned char g_810e[];
extern unsigned short g_10e[];
extern short g_10b5a, g_10b5c, g_10b5e;

unsigned short *FUN_00034088(void)
{
    unsigned short head = g_10e[((0x7f00 & g_10b5c) >> 1) | ((g_10b5e >> 8) & 0x7f)];
    while (head != 0) {
        unsigned char *node = g_810e + head;
        if (!(1 & node[0xb]) && !(node[0xa] & 1) &&
            (0xff00 & *(short *)(8 + node)) == (g_10b5a & 0xff00)) {
            if (node[0x18] == 2)
                return (unsigned short *)node;
            if (node[0x18] == 1)
                return (unsigned short *)node;
        }
        head = *(unsigned short *)node;
    }
    return 0;
}
