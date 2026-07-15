/* NEAR-MISS @ 0x16318 -- 280/287 masked (was ~242/287); PARKED on ONE window:
 * entry order. Target loads g_cur_player into SI BEFORE the param byte
 * (mov ch,[esp+0x10]); ours always emits the param load first. Failed flips
 * (this session + prior): volatile on/off (non-volatile SINKS the SI load to
 * first use mid-chain -- worse), decl order, statement-vs-initializer form,
 * gg copy-local, save-first compare (flips the cmp to 39c6 cmp si,ax; target
 * 39f0 needs the table value as source LHS), and the -or recipe (skips no
 * such hoist here; shl-multiplies, 273B, still param-first). Entry-scheduler
 * internal -- same family as the 0x264a8/0x35d08 walls.
 * The OLD window (2) is CLOSED by the cont.21 named-temp lever: a block-scoped
 * `unsigned int idx = g * 19 + i;` homes the index in EAX and the multiply
 * chain in EDX exactly like the target (8d149d/01da/31c0/8d1495/88c8/29da),
 * killing ours' mov edx,ebx / xor edx,ebx zeroing quirk (-2B). Note the
 * inverse ordering: `t = i;` alone SPILLS t and loses push edi; the single
 * whole-index temp is the correct granularity. (unsigned int)i casts and
 * i/g*19 commutes are byte-inert.
 *
   0x16318 -- research funding tick for group g. Records are 10 bytes at
 * 0x539c: word +0 funding, byte +2 owner, byte +3 rate. If we own g
 * (owner == g_cur_player): for each of the 8 links in g_b069[g*19 + i], a nonzero
 * link owned by someone else adds +2 funding. Then funding drifts by
 * (rate - 30)/2, nudges 2 toward 30, and clamps to 0..0xff. g_cur_player is
 * saved in ESI and restored at the end (unmodified).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern volatile short g_cur_player;
extern unsigned char g_539c[];
extern unsigned char g_539e[];
extern unsigned char g_b069[];

void FUN_00016318(unsigned char g)
{
    unsigned char i;
    short save;

    save = g_cur_player;
    if (g_539e[g * 10] == save) {
        for (i = 0; i != 8; i++) {
            unsigned int idx = g * 19 + i;
            if (g_b069[idx] != 0
                && g_539e[g_b069[idx] * 10] != save)
                *(short *)(g_539c + g * 10) += 2;
        }
    }
    *(short *)(g_539c + g * 10) += ((signed char)g_539c[g * 10 + 3] - 0x1e) / 2;
    if (*(short *)(g_539c + g * 10) > 0x1e)
        *(short *)(g_539c + g * 10) -= 2;
    else if (*(short *)(g_539c + g * 10) < 0x1e)
        *(short *)(g_539c + g * 10) += 2;
    if (*(short *)(g_539c + g * 10) < 0)
        *(short *)(g_539c + g * 10) = 0;
    else if (*(short *)(g_539c + g * 10) > 0xff)
        *(short *)(g_539c + g * 10) = 0xff;
    g_cur_player = save;
}
