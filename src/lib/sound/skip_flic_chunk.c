/* frameless linear call @ 0x39f49: g(g_bdd0, g_back_buf, (u16)g_bddc - 6) */
extern unsigned short g_bddc;
extern int g_back_buf;
extern int g_bdd0;
extern void read(int a, int b, int c);
void skip_flic_chunk(void)
{
    read(g_bdd0, g_back_buf, g_bddc - 6);
}
