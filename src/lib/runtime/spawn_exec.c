/* framed two-call @ 0x3cc45:
   g1(b, e, c, 0); g2(a, b, c, d, e);  params a=+8 b=+c c=+10 d=+14 e=+18 */
extern void build_cmdtail(int b, int e, int c, int z);
extern void dos_exec(int a, int b, int c, int d, int e);
void spawn_exec(int a, int b, int c, int d, int e)
{
    build_cmdtail(b, e, c, 0);
    dos_exec(a, b, c, d, e);
}
