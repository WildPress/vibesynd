/* frameless @ 0x1a8c8: fill a 0x19-row x 0x10-col byte grid (row stride 0x10) at g_minimap
   with 1, then set the flag g_minimap_ready. Outer counter y (ECX) 0..0xf, inner x (EAX) 0..0x18;
   address = x*0x10 + y + g_minimap (x sign-extended then <<4). */
extern unsigned char g_minimap[];
extern unsigned char g_minimap_ready;
void fill_minimap_grid(void)
{
    short y = 0;
    do {
        short x = 0;
        do {
            g_minimap[y + x * 0x10] = 1;
            x++;
        } while (x < 0x19);
        y++;
    } while (y < 0x10);
    g_minimap_ready = 1;
}
