/* @ 0x22728 -- init call sequence with a conditional third step. */
extern void walk_records_2c(int a);
extern void FUN_00036188(void);
extern char g_sound_enabled;

void init_call_seq(void)
{
    walk_records_2c(0x3f08);
    walk_records_2c(0x4144);
    if (g_sound_enabled != 0) {
        FUN_00036188();
        walk_records_2c(0x40c0);
    }
}
