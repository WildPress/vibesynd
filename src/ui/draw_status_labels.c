/* frameless two table-lookup calls @ 0x29a28 */
extern unsigned char g_language;
extern int tbl_4468[];
extern int tbl_4474[];
extern void center_string_16(int a, int b);
void draw_status_labels(void)
{
    center_string_16(0x10564, tbl_4468[g_language]);
    center_string_16(0x10574, tbl_4474[g_language]);
}
