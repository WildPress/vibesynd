/* 0x1b908 -- walk the 10-byte record chain for slot `idx` (head index in the
 * 8-byte table at g_5338, records at g_5340, next-link at +8), and for each
 * record schedule FUN_0004a63a(x+rec[2], y+rec[4], g_5314+dur, rec[6]) with
 * g_5314 reset to its entry value before each call and re-read after.
 * Duration: if rec[0]/6-0x29 in [0,8) it's remapped through the per-actor
 * table g_e4ab (actor id = (p-0x8110)/0x5c/8, 0x417-byte stride), else rec[0]
 * raw. Recipe: -4s -oneatx -zp8 -s -zq.
 */
extern unsigned char *g_5338;
extern unsigned char *g_5340;
extern unsigned char *g_5340b; /* alias symbol for the compares (differ masks fixups) */
extern volatile unsigned int g_5314;
extern unsigned char g_e4ab[];

extern void FUN_0004a63a(int x, int y, unsigned int end, int arg4);

void FUN_0001b908(int p, unsigned short idx, int x, int y)
{
    int off;
    unsigned char *rec;
    register unsigned int start;

    start = g_5314;
    rec = g_5340 + *(unsigned short *)(idx * 8 + (int)g_5338) * 10;
    if (rec > g_5340b) {
        off = (p - 0x8110) / 0x5c / 8 * 0x417;
        do {
            unsigned short b;
            unsigned short base;
            unsigned short t;
            int v;
            unsigned int end;

            b = g_e4ab[off];
            t = *(volatile unsigned short *)rec / 6 - 0x29;
            base = b * 8 + 0x461;
            if (t < 8)
                v = ((t & 7) + base) * 6;
            else
                v = *(volatile unsigned short *)rec;
            end = v + start;
            g_5314 = start;
            if (end > start)
                FUN_0004a63a((short)(x + *(unsigned short *)(rec + 2)),
                             (short)(y + *(unsigned short *)(rec + 4)),
                             end,
                             *(unsigned short *)(rec + 6));
            start = g_5314;
            rec = g_5340 + *(unsigned short *)(rec + 8) * 10;
        } while (rec > g_5340b);
    }
    g_5314 = start;
}
