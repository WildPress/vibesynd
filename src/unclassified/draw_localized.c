/* frameless table-lookup call @ 0x29a68: g(0x10564, tbl_4480[g_language]) */
extern unsigned char g_language;
extern int tbl_4480[];
extern void center_string_16(int a, int b);
void draw_localized(void)
{
    center_string_16(0x10564, tbl_4480[g_language]);
}
