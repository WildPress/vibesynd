/* frameless @ 0x1ff98: draw a gauge/bar segment. Params: x,y (u16 screen coords),
   a (total/divisor), b (current value). Clamp b to [0,a]; percentage v = b*0x18/a
   (0..0x18), with a==0 giving 0x18. Then call FUN_0003fb40(x+4, y+0x16, v, 6, 0xc). */
extern int FUN_0003fb40();

void draw_gauge_segment(unsigned short x, unsigned short y, short a, short b)
{
    int v;
    if (b < 0)
        return;
    if (b > a)
        b = a;
    if (a == 0) {
        if (b >= 0)
            v = 0x18;
        else
            v = 0;
    } else {
        v = b * 0x18 / a;
    }
    FUN_0003fb40(x + 4, y + 0x16, (unsigned short)v, 6, 0xc);
}
