/* frameless @ 0x1a8c8: fill a 0x19-row x 0x10-col byte grid (row stride 0x10) at g_db2c
   with 1, then set the flag g_10b4f. Outer counter y (ECX) 0..0xf, inner x (EAX) 0..0x18;
   address = x*0x10 + y + g_db2c (x sign-extended then <<4). */
extern unsigned char g_db2c[];
extern unsigned char g_10b4f;
void FUN_0001a8c8(void)
{
    short y = 0;
    do {
        short x = 0;
        do {
            g_db2c[y + x * 0x10] = 1;
            x++;
        } while (x < 0x19);
        y++;
    } while (y < 0x10);
    g_10b4f = 1;
}
