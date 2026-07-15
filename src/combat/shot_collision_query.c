/* frameless @ 0x34088: collision query along a shot path. The shot's accumulated
   position lives in the globals g_shot_x (x), g_shot_y (y), g_shot_level (level). Build the
   128x128 spatial-grid cell index from the high bytes of x,y and walk the object
   linked list rooted at g_grid_heads[idx]. Return the first object (node = g_entity_pool + id) that
   is not flagged (byte[0xb]&1, byte[0xa]&1 both clear), sits on the same level (high
   byte of node[8] == high byte of g_shot_level), and is of type 1 or 2. NULL if none.
   g_shot_level is read every iteration, so -oa homes it in CX and writes it back at exit.

   PARKED near-miss (128/129, NOT matched -> not in recipes/manifest). Everything
   matches -- the idx build, the CX cache+writeback, the flag/level/type checks, the
   register layout of the level compare (node8 in EBX, level in EDX) -- except ONE
   byte: the CMP modrm. The target encodes CMP EDX,EBX (reg field = node8, the
   memory operand, 39 da); Watcom gives us CMP EBX,EDX (reg field = the second
   operand, 39 d3) for every C spelling. Confirmed: both operand orders and a node8
   temp all reproduce the same d3, and 8000 cpermute variants plateau at 128/129. A
   pure compare-encoding tie-break, same class as 0x34048 / 0x26e18. */
extern unsigned char g_entity_pool[];
extern unsigned short g_grid_heads[];
extern short g_shot_level, g_shot_y, g_shot_x;

unsigned short *shot_collision_query(void)
{
    unsigned short head = g_grid_heads[((0x7f00 & g_shot_y) >> 1) | ((g_shot_x >> 8) & 0x7f)];
    while (head != 0) {
        unsigned char *node = g_entity_pool + head;
        if (!(1 & node[0xb]) && !(node[0xa] & 1) &&
            (0xff00 & *(short *)(8 + node)) == (g_shot_level & 0xff00)) {
            if (node[0x18] == 2)
                return (unsigned short *)node;
            if (node[0x18] == 1)
                return (unsigned short *)node;
        }
        head = *(unsigned short *)node;
    }
    return 0;
}
