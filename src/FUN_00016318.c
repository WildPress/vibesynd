/* NEAR-MISS @ 0x16318 -- ~95%; PARKED on two register-role windows.
 * (1) Entry order: target loads g_10b16 into SI before the param byte
 * (mov ch,[esp+0x10]); ours always hoists the param load first -- volatile on
 * g_10b16, decl order, and a gg copy-local all fail to flip it.
 * (2) The g*19+i index chain homes the multiply in EDX/i in EAX in the
 * target, EAX/EDX in ours; commutes, g*20-g respelling, and 2500 cpermute
 * variants keep the rotated form (best 242/287 alignment). Everything else
 * matches masked, including the inline compound drift/clamp (part-3 lever:
 * copy-temps in EDI/EDX for the +-2 branches came from inlining the memory
 * expression; a named w gave in-place ops and lost push edi).
 *
   0x16318 -- research funding tick for group g. Records are 10 bytes at
 * 0x539c: word +0 funding, byte +2 owner, byte +3 rate. If we own g
 * (owner == g_10b16): for each of the 8 links in g_b069[g*19 + i], a nonzero
 * link owned by someone else adds +2 funding. Then funding drifts by
 * (rate - 30)/2, nudges 2 toward 30, and clamps to 0..0xff. g_10b16 is
 * saved in ESI and restored at the end (unmodified).
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern volatile short g_10b16;
extern unsigned char g_539c[];
extern unsigned char g_539e[];
extern unsigned char g_b069[];

void FUN_00016318(unsigned char g)
{
    short save = g_10b16;
    unsigned char i;

    if (g_539e[g * 10] == save) {
        for (i = 0; i != 8; i++) {
            if (g_b069[i + g * 19] != 0
                && g_539e[g_b069[i + g * 19] * 10] != save)
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
    g_10b16 = save;
}
