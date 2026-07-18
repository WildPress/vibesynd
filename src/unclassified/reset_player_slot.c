/* reset_player_slot @ 0x22ed8 (159 bytes)
 * Reset the per-index slot for index param_1: clear a 0x417-byte block,
 * zero the 0xe-stride record fields, stamp the id byte / flag, and set the
 * per-index angle (param<<3) and a word copied from g_10b2e.
 * Watcom C 9.5b, recipe -4s -oneatx -zp8 -s -zq.
 */

extern void FUN_0004d1db(void *dst, void *src, unsigned int len);
extern unsigned char g_player_recs[];
extern unsigned char g_command_recs[];
extern unsigned short g_10b2e;

void reset_player_slot(unsigned short param_1)
{
    FUN_0004d1db(g_player_recs, g_player_recs + param_1 * 0x417, 0x417);

    *(unsigned short *)(g_command_recs + param_1 * 0xe + 0) = 0;
    *(unsigned short *)(g_command_recs + param_1 * 0xe + 2) = 0;
    *(unsigned short *)(g_command_recs + param_1 * 0xe + 4) = 0;
    *(unsigned short *)(g_command_recs + param_1 * 0xe + 0xa) = 0;
    *(unsigned short *)(g_command_recs + param_1 * 0xe + 6) = 0;
    *(unsigned char *)(g_command_recs + param_1 * 0xe + 0xc) = (char)param_1;
    *(unsigned char *)(g_command_recs + param_1 * 0xe + 0xd) = 1;

    *(char *)(g_player_recs + param_1 * 0x417 + 0xb5) = (char)param_1 << 3;
    *(unsigned short *)(g_player_recs + param_1 * 0x417 + 0xb7) = g_10b2e;
}
