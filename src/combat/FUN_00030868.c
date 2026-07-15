/* frameless @ 0x30868: (re)acquire + engage target for pool-A entity `node`.
   Clears node[0x54], d = FUN_2d808(node,0x64) (kept in DI). Clears node[0x46];
   if the target link node[0x44] is set: p2 = its pool node; if p2 health word
   +0x14 < 0, reselect via FUN_37ad8(node,0) — same id => FUN_2d998(node), else
   store the new link. Then weapon range = g_a6c2[p2 type]/0x100 (ushort cast),
   g_e12e = FUN_2d7a8(node, range) << 8, and q = FUN_2ee18(node, g_e12e, d)
   scans for a valid target; on hit aim at (q+4, q+6, q+8 + 0x80) via FUN_2f608.
   Tail: FUN_269d8(node); if counter node[0x42]==0 FUN_2d998(node); decrement
   the counter; node[0x19] = FUN_2dd48(node).

   PARKED at 259/261: stride-2 table-index CANONICALIZATION wall. Target indexes
   the s16 table 0xa6c2 via `xor edx,edx; mov dl,type; add edx,edx;
   mov dx,[edx+0xa6c2]; and edx,0xffff` (doubled index as a VALUE + disp32 +
   named-ushort and-widen, 22B); ours always folds the doubling into SIB
   `[eax*2+sym]` (20B) � 7 spellings tried (array index, byte-array *2, <<1,
   x+x CSE, stride-2 struct, cross-statement int/uint/ushort offset locals,
   compound self-add). Note the binary DOES use the SIB form for the same table
   elsewhere (0x23158), so the original source spelled this site differently in
   a way 9.5 no longer distinguishes for us. Everything else byte-matches. */
extern unsigned char g_entity_pool[];
extern unsigned char g_a6c2[];
extern short g_e12e;
extern int interp_scale_b(unsigned char *node, int cap);
extern unsigned short FUN_00037ad8(unsigned char *node, int flag);
extern void FUN_0002d998(unsigned char *node);
extern int interp_scale_a(unsigned char *node, int range);
extern unsigned char *FUN_0002ee18(unsigned char *node, int dist, int d);
extern void entity_aim_helper(unsigned char *node, int x, int y, int z);
extern void FUN_000269d8(unsigned char *node);
extern unsigned char entity_event_dispatch(unsigned char *node);

void FUN_00030868(unsigned char *node)
{
    short d;
    unsigned short t;
    unsigned short w;
    unsigned int off;
    unsigned char *p2;
    unsigned char *q;

    node[0x54] = 0;
    d = interp_scale_b(node, 0x64);
    node[0x46] = 0;
    if (*(unsigned short *)(node + 0x44) != 0) {
        p2 = g_entity_pool + *(unsigned short *)(node + 0x44);
        if (*(short *)(p2 + 0x14) < 0) {
            t = FUN_00037ad8(node, 0);
            if (t == *(unsigned short *)(node + 0x44))
                FUN_0002d998(node);
            else
                *(unsigned short *)(node + 0x44) = t;
        }
        off = p2[0x19];
        off += off;
        w = *(unsigned short *)(g_a6c2 + off);
        g_e12e = interp_scale_a(node, (unsigned short)(w / 0x100)) << 8;
        q = FUN_0002ee18(node, g_e12e, d);
        if (q != 0) {
            entity_aim_helper(node, *(short *)(q + 4), *(short *)(q + 6),
                         (short)(*(unsigned short *)(q + 8) + 0x80));
        }
    }
    FUN_000269d8(node);
    if (*(unsigned short *)(node + 0x42) == 0)
        FUN_0002d998(node);
    *(unsigned short *)(node + 0x42) -= 1;
    node[0x19] = entity_event_dispatch(node);
}
