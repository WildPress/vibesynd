/* framed two-call @ 0x3cc45:
   g1(b, e, c, 0); g2(a, b, c, d, e);  params a=+8 b=+c c=+10 d=+14 e=+18 */
extern void FUN_0003e143(int b, int e, int c, int z);
extern void FUN_0003e1af(int a, int b, int c, int d, int e);
void FUN_0003cc45(int a, int b, int c, int d, int e)
{
    FUN_0003e143(b, e, c, 0);
    FUN_0003e1af(a, b, c, d, e);
}
