/* frameless @ 0x16438 (132B): per-index timer/percent computation.  Records are
   stride 10 based at g_syndicate_recs, indexed by param_1*10.  If the record's byte field
   (+2, 0x539e) equals the word g_cur_player AND the record selected by g_10b36 has its
   +0 word (0x539c) < 0xa0, compute (f6/1000000)*14*(signed)f3/10 and refresh f3
   from f4; otherwise return 0. */
#pragma pack(1)
struct rec {
    short f0;           /* +0  0x539c */
    unsigned char f2;   /* +2  0x539e */
    signed char f3;     /* +3  0x539f */
    unsigned char f4;   /* +4  0x53a0 */
    unsigned char f5;   /* +5 */
    unsigned int f6;    /* +6  0x53a2 */
};

extern struct rec g_syndicate_recs[];
extern unsigned short g_cur_player;
extern unsigned char g_10b36;

unsigned int FUN_00016438(unsigned char param_1)
{
    unsigned int result;
    int t;

    result = 0;
    t = g_syndicate_recs[param_1].f2;
    if ((unsigned short)t == g_cur_player && g_syndicate_recs[g_10b36].f0 < 0xa0) {
        result = (g_syndicate_recs[param_1].f6 / 1000000) * 14 * g_syndicate_recs[param_1].f3 / 10;
        g_syndicate_recs[param_1].f3 = g_syndicate_recs[param_1].f4;
    }
    return result;
}
