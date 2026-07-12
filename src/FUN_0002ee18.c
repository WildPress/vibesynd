/* NEAR-MISS @ 0x2ee18 -- SEMANTICS FULLY DECODED; parked on a loop-invariant
 * hoist wall. Find a valid target for agent a: scans pool-A records (0x5c bytes
 * at g_8110, end g_10ae0), skipping the 8-record squad that a itself belongs to
 * (group index di = ((a - g_810e)/0x5c) & 0xf8). A record is eligible when
 * in-use (+0x18==1), has a link (+0x44!=0) or the +0x1c bit-1 flag, isn't
 * flagged 0x109 at +0xa, and has +0x20==0. For each, 0x2e5f8(a, rec, p2)
 * returning rec means hit -> return rec. Returns 0 if none or p3<=0.
 *
 * WALL: the target recomputes `di+8` INSIDE the loop each iteration
 * (mov ax,di; add eax,8; cmp dx,ax) using all four callee-saved regs
 * (ebx=rec, esi=si, edi=di, ebp=a). -oneatx HOISTS the loop-invariant di+8 to
 * a stack slot (forcing sub esp,4 + a per-iteration reload); `int di` gets the
 * ebp allocation but keeps the spill. Rewriting the guard as (ushort)(si-di)>=8
 * collapses the two compares (wrong shape); -oneat drops the hoist but changes
 * other codegen. No lever recomputes the invariant while keeping the two-compare
 * form. Same class as 0x26da8/0x269d8. Recipe would be -4s -oneatx -zp8 -s -zq.
 */
extern unsigned char g_810e[];
extern unsigned char g_8110[];
extern unsigned char *g_10ae0;
extern unsigned char *FUN_0002e5f8(unsigned char *a, unsigned char *rec, short p);

unsigned char *FUN_0002ee18(unsigned char *a, short p2, short p3)
{
    int di;
    unsigned short si = 0;
    unsigned char *rec = g_8110;

    if (p3 <= 0)
        return 0;
    di = (unsigned char)((unsigned)(unsigned short)(a - g_810e) / 0x5c) & 0xf8;
    if (g_10ae0 <= rec)
        return 0;
    do {
        if (si < di || si >= (unsigned short)(di + 8)) {
            if (rec[0x18] == 1
                && (*(unsigned short *)(rec + 0x44) != 0 || (rec[0x1c] & 2))
                && !(*(unsigned short *)(rec + 0xa) & 0x109)
                && *(unsigned short *)(rec + 0x20) == 0) {
                if (FUN_0002e5f8(a, rec, p2) == rec)
                    return rec;
            }
        }
        rec += 0x5c;
        si++;
    } while (rec < g_10ae0);
    return 0;
}
