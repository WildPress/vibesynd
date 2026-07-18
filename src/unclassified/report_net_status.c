/* frameless 5-arg call @ 0x289a8: g(0x37d8, p1, (u16)p2, (short)p3, (short)p3) */
extern void printf(int a, int b, int c, int d, int e);
void report_net_status(int p1, unsigned short p2, short p3)
{
    printf(0x37d8, p1, p2, p3, p3);
}
