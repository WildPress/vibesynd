/* 0x1b908 -- walk the 10-byte record chain for slot `idx` (head index in the
 * 8-byte table at g_rec8_table, records at g_5340, next-link at +8), and for each
 * record schedule FUN_0004a63a(x+rec[2], y+rec[4], start+dur, rec[6]) with
 * g_5314 (time cursor) reset to `start` before each call and re-read after.
 * Duration: if rec[0]/6-0x29 in [0,8) it's remapped via the per-actor byte
 * table g_e4ab (actor id = (p-0x8110)/0x5c/8, stride 0x417), else rec[0] raw.
 * Recipe: -4s -oneatx -zp8 -s -zq.
 *
 * PARKED at 313/317 (true size 317; manifest says 315 -- undercounted, Ghidra
 * mis-parses the bytes after the CALL). Pure register-role wall: ours homes
 * start=ECX / base=EDI, target has start=EDI / base=ECX; everything else is
 * byte-identical (the swap cascades into head-copy ESI vs ECX, tail-copy EDI
 * vs EBP, `mov ebp,6` hoisted vs in-loop, and the two widen forms in the
 * if-arm). First diff at 0x5 (modrm of the entry `mov edi,[g_5314]`).
 */
extern unsigned char *g_rec8_table;
extern unsigned char *g_5340;
extern unsigned char *g_5340b; /* alias symbol for the compares (differ masks fixups) */
extern volatile unsigned int g_5314;
extern unsigned char g_e4ab[];

extern void FUN_0004a63a(int x, int y, unsigned int end, int arg4);

void FUN_0001b908(int p, unsigned short idx, int x, int y)
{
    unsigned short base;
    unsigned short t;
    int v;
    unsigned int end;
    int off;
    unsigned char *rec;
    unsigned int start;

    start = g_5314;
    rec = g_5340 + *(unsigned short *)(idx * 8 + (int)g_rec8_table) * 10;
    if (rec > g_5340b) {
        off = (p - 0x8110) / 0x5c / 8 * 0x417;
        do {
            base = g_e4ab[off] * 8 + 0x461;
            t = *(volatile unsigned short *)rec / 6 - 0x29;
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
