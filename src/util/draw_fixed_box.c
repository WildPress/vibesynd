/* 0x15e08 -- 5-arg constant forwarder into draw_box_outline.
 * xor eax,eax; mov al,[esp+4] = unsigned char param widened.
 * Push order: a, 0x96, 0x264, 0x16, 0x118 -> args (0x118,0x16,0x264,0x96,a).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern void draw_box_outline(int a, int b, int c, int d, int e);

void draw_fixed_box(unsigned char a)
{
    draw_box_outline(0x118, 0x16, 0x264, 0x96, a);
}
