/* frameless @ 0x1a918: mark a clamped rectangle of the minimap grid g_minimap with 3.
   Convert the four screen coords to grid cells in place: param_1=(p1-0x80)/0x20,
   param_3=(p3-0x80)/0x20 (rows), param_2=p2/0x10, param_4=p4/0x10 (cols). Bail unless
   param_4>=0 && param_2<0x19 && param_1<0x10 && param_3>=0. Clamp, then for each row
   param_1..param_3 and col param_2..param_4 set g_minimap[row + col*0x10]=3. (param_3 stays
   on the stack; the other three coords live in EBX/EDI/ESI.) */
extern unsigned char g_minimap[];
void minimap_mark_rect(short param_1, short param_2, short param_3, short param_4)
{
    short row, col;
    param_1 = (param_1 - 0x80) / 0x20;
    param_3 = (param_3 - 0x80) / 0x20;
    param_2 = param_2 / 0x10;
    param_4 = param_4 / 0x10;
    if (param_4 >= 0 && param_2 < 0x19 && param_1 < 0x10 && param_3 >= 0) {
        if (param_2 < 0) param_2 = 0;
        if (param_4 >= 0x19) param_4 = 0x18;
        if (param_1 < 0) param_1 = 0;
        if (param_3 >= 0x10) param_3 = 0xf;
        row = param_1;
        if (row <= param_3) {
            do {
                col = param_2;
                if (col <= param_4) {
                    do {
                        g_minimap[row + col * 0x10] = 3;
                        col++;
                    } while (col <= param_4);
                }
                row++;
            } while (row <= param_3);
        }
    }
}
