/* frameless linear call @ 0x39f69: g(g_bdd0, g_10ab0, (u16)g_bddc - 6); g_be2e = 1
   0x39xxx unit is -3 (386) AND a lighter opt than the main game: compile "-3s" with
   NO -oneatx (plain -3s -zp8 -s -zq), or -oneatx reorders the store past the cleanup. */
extern unsigned short g_bddc;
extern int g_10ab0;
extern int g_bdd0;
extern unsigned short g_be2e;
extern void read(int a, int b, int c);
void read_flic_palette(void)
{
    read(g_bdd0, g_10ab0, g_bddc - 6);
    g_be2e = 1;
}
