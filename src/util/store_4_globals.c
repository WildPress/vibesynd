/* frameless global setter @ 0x18ab8: store 4 args into 4 consecutive globals */
extern int g_clip_x, g_clip_y, g_clip_w, g_clip_h;
void store_4_globals(int a, int b, int c, int d)
{
    g_clip_x = a;
    g_clip_y = b;
    g_clip_w = c;
    g_clip_h = d;
}
