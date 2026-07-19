/* frameless global setter @ 0x18ab8: store 4 args into 4 consecutive globals */
extern int g_5380, g_5384, g_5388, g_538c;
void store_4_globals(int a, int b, int c, int d)
{
    g_5380 = a;
    g_5384 = b;
    g_5388 = c;
    g_538c = d;
}
