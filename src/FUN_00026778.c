/* @ 0x26778 (544 bytes): dashed Bresenham line drawer, plotting every 2nd pixel
   from (x1,y1) to (px,py) via the pixel-plot callee FUN_00040236(x,y,color).
   Color alternates between c1 and c2 on bit 2 of the ushort pattern counter a7
   (a7 is decremented per step on shallow lines, incremented on steep ones, so
   4-pixel dashes of each color). Classic two-branch integer Bresenham: shallow
   (|dx| >= |dy|) walks x in +/-2 steps stepping y by +/-2 on error overflow;
   steep mirrors with x/y swapped. Coordinates are treated as unsigned shorts. */

extern void FUN_00040236(int x, int y, int c);

void FUN_00026778(int x1, int y1, int x2, int y2,
                  unsigned char c1, unsigned char c2, unsigned short a7)
{
    int stepa;
    int stepb;
    int inc1a;
    int inc2a;
    int inc1b;
    int inc2b;
    int end1;
    int end2;
    int i;
    int j;
    int two;
    int err;
    int dx;
    int dy;
    int px = x2;
    int py = y2;

    two = 2;
    dx = (unsigned short)px - (unsigned short)x1;
    if (dx < 0) {
        dx = -dx;
    }
    dy = (unsigned short)py - (unsigned short)y1;
    if (dy < 0) {
        dy = -dy;
    }
    if (dx >= dy) {
        inc2a = dy * 2;
        err = dy * 2 - dx;
        inc1a = (dy - dx) * 2;
        if ((unsigned short)px < (unsigned short)x1) {
            i = (unsigned short)px;
            end1 = (unsigned short)x1;
            j = (unsigned short)py;
            if ((unsigned short)py > (unsigned short)y1) {
                stepa = two;
                stepa = -stepa;
            } else {
                stepa = two;
            }
        } else {
            i = (unsigned short)x1;
            end1 = (unsigned short)px;
            j = (unsigned short)y1;
            if ((unsigned short)py < (unsigned short)y1) {
                stepa = two;
                stepa = -stepa;
            } else {
                stepa = two;
            }
        }
        FUN_00040236(i, j, (a7 & 4) ? c2 : c1);
        while (i < end1) {
            i += two;
            if (err < 0) {
                err += inc2a;
            } else {
                j += stepa;
                err += inc1a;
            }
            FUN_00040236(i, j, (a7 & 4) ? c2 : c1);
            a7--;
        }
    } else {
        inc2b = dx * 2;
        err = dx * 2 - dy;
        inc1b = (dx - dy) * 2;
        if ((unsigned short)py < (unsigned short)y1) {
            i = (unsigned short)py;
            end2 = (unsigned short)y1;
            j = (unsigned short)px;
            if ((unsigned short)px > (unsigned short)x1) {
                stepb = two;
                stepb = -stepb;
            } else {
                stepb = two;
            }
        } else {
            i = (unsigned short)y1;
            end2 = (unsigned short)py;
            j = (unsigned short)x1;
            if ((unsigned short)px < (unsigned short)x1) {
                stepb = two;
                stepb = -stepb;
            } else {
                stepb = two;
            }
        }
        FUN_00040236(j, i, (a7 & 4) ? c2 : c1);
        while (i < end2) {
            i += two;
            if (err < 0) {
                err += inc2b;
            } else {
                j += stepb;
                err += inc1b;
            }
            FUN_00040236(j, i, (a7 & 4) ? c2 : c1);
            a7++;
        }
    }
}
