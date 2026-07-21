/* MATCHED @ 0x2ee18 (163/163, reloc-aware) -- recipe -4s -oneatx -zp8 -s -zq.
 * Find a valid target for agent a: scans pool-A records (0x5c bytes at g_pool_a,
 * end g_pool_a_free), skipping the 8-record squad that a itself belongs to
 * (group index di = ((a - g_entity_pool)/0x5c) & ~7). A record is eligible when
 * in-use (+0x18==1), has a link (+0x44!=0) or the +0x1c bit-1 flag, isn't
 * flagged 0x109 at +0xa, and has +0x20==0. For each, 0x2e5f8(a, rec, p2)
 * returning rec means hit -> return rec. Returns 0 if none or p3<=0.
 *
 * The old "loop-invariant di+8 hoist wall" fell to the cont.21 lever set:
 * (1) di retyped unsigned short + the `di + 8` written INLINE in the guard --
 *     int promotion keeps the two-compare form (16-bit `cmp si,di; jb` then
 *     32-bit `mov ax,di; add eax,8; cmp edx,eax; jl`) and LICM re-materialises
 *     it per iteration instead of spilling a hoisted slot (sub esp,4 gone);
 * (2) si/rec inits moved from decl-initializers to STATEMENTS after the p3
 *     guard -- gives the slot-compare `cmp word [esp+0x1c],0` entry and the
 *     div-by-EBX/rec-load-after-div schedule;
 * (3) the group mask is `& ~7` (NOT `(uchar)... & 0xf8`): high mask bits all
 *     set => Watcom demanded-bits narrows the AND to `and al,0xf8` and copies
 *     the full reg (`mov edi,eax`, quotient bits 8-15 pass through) -- the
 *     uchar-cast spelling inserts a movzx, the 0xf8/0xfff8 int masks stay wide.
 */
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char *g_pool_a_free;
extern unsigned char *los_trace(unsigned char *a, unsigned char *rec, short p);

unsigned char *find_target_for_agent(unsigned char *a, short p2, short p3)
{
    unsigned short di;
    unsigned short si;
    unsigned char *rec;

    if (p3 <= 0)
        return 0;
    di = (unsigned)(unsigned short)(a - g_entity_pool) / 0x5c & ~7;
    si = 0;
    rec = g_pool_a;
    if (g_pool_a_free <= rec)
        return 0;
    do {
        if (si < di || si >= di + 8) {
            if (rec[0x18] == 1
                && (*(unsigned short *)(rec + 0x44) != 0 || (rec[0x1c] & 2))
                && !(*(unsigned short *)(rec + 0xa) & 0x109)
                && *(unsigned short *)(rec + 0x20) == 0) {
                if (los_trace(a, rec, p2) == rec)
                    return rec;
            }
        }
        rec += 0x5c;
        si++;
    } while (rec < g_pool_a_free);
    return 0;
}
