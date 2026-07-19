/* frameless @ 0x28cc8: build two local buffers, zero a word in one, call
   int386(0x33, b, a); then if global g_df3c is set, call
   free(g_df3c). */
extern int int386(int a, void *b, void *c);
extern void free(int x);
extern int g_df3c;
void build_two_buffers(void)
{
    char b[0x1c];
    char a[0x1c];
    *(short *)b = 0;
    int386(0x33, b, a);
    if (g_df3c != 0)
        free(g_df3c);
}
