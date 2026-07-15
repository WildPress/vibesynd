/* frameless @ 0x37918: drop/scatter the chain of carried items hanging off
   entity p (head id at p+0x3a, links at node+0x1c). Clears the carry fields
   (p+0x3a, p+0x46, p+0x44), then for each chained node: unlink (clear +0x1e,
   +0x20, +0x1c, flag bit0 of +0xa), pick a random offset inside p's tile
   (low-byte coord + rng(0xff) - 0x64, wrapped into [0,0x100) by +-0x64), and
   move the node there at floor height via FUN_26c78(node, x, y, FUN_fa18(x,y,pz)).

   PARKED at 286/304 (first diff 0x7): register-role rotation � target homes
   p->ESI, node->EBX, y->EDI with x slot-homed; ours p->EDI + rotated others.
   `volatile int x` reproduces the target's slot-homed x (every access via
   [esp+4]); decl orders inert; 4000 cpermute variants no match. */
extern unsigned char g_entity_pool[];
extern int lcg_rand(int cap);
extern int FUN_0000fa18(int x, int y, int z);
extern void move_entity_xyz(unsigned char *node, int x, int y, int z);

void drop_carried_items(unsigned char *p)
{
    unsigned short id;
    volatile int x;
    unsigned char *node;
    int y;

    id = *(unsigned short *)(p + 0x3a);
    if (id != 0) {
        *(unsigned short *)(p + 0x3a) = 0;
        p[0x46] = 0;
        *(unsigned short *)(p + 0x44) = 0;
        while (id != 0) {
            node = g_entity_pool + id;
            *(unsigned short *)(node + 0x1e) = 0;
            id = *(unsigned short *)(node + 0x1c);
            *(unsigned short *)(node + 0x20) = 0;
            *(unsigned short *)(node + 0x1c) = 0;
            node[0xa] &= 0xfe;
            x = (*(unsigned short *)(p + 4) & 0xff) + lcg_rand(0xff) - 0x64;
            y = (*(unsigned short *)(p + 6) & 0xff) + lcg_rand(0xff) - 0x64;
            if ((short)x >= 0x100)
                x -= 0x64;
            if ((short)y >= 0x100)
                y -= 0x64;
            if ((short)x < 0)
                x += 0x64;
            if ((short)y < 0)
                y += 0x64;
            move_entity_xyz(node,
                         (short)((*(volatile unsigned short *)(p + 4) & 0xff00) + x),
                         (short)((*(volatile unsigned short *)(p + 6) & 0xff00) + y),
                         (short)FUN_0000fa18(
                             (short)((*(unsigned short *)(p + 4) & 0xff00) + x),
                             (short)((*(unsigned short *)(p + 6) & 0xff00) + y),
                             *(short *)(p + 8)));
        }
    }
}
