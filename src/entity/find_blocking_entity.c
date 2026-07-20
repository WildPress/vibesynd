/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): audited against a clean objdump of
   the target region (0x11d68..0x128a8). Every behaviourally-load-bearing element matches
   the target exactly: box-slack constants (0x20 x11, 0x40 x20-arith, 0x80 x19), z-offsets
   (0x100 x10, 0x170/0x180/0x190 x1 each), the subtype>0x2a miss bound, all node struct
   offsets (+4,+6,+8,+0xa,+0xb,+0x18,+0x19,+0x1c,+0x20), the type-1 squad/leader checks,
   and both inclusive z boundaries (entry dy<=0; case 0x26/27/29 node8<=z+rz -> return).
   The signed-compare total is identical (jl+jle+jg+jge = 69 both sides); the jl<->jg
   redistribution is per-body cmp operand-orientation (a<b == b>a) and the case-0x26 <=
   is a jg-to-skip vs jle-to-return polarity flip -- both behaviour-preserving. Residual
   tie class: frame 0x84 vs 0x8c, node in EBP (target) vs EBX (ours), ~33 spill-slot
   assignments + one extra [esp+0x40] reload, and the jump-table index zero-extension
   form. No wrong constant/offset/operator/bound found.
   -- PARKED near-miss @ 0x11d68 -- EDIT-DIST 1125 (ours 3323B vs target 2881B,
   ~66% by _probe.sh); recipe -4s -oneatx -zp8 -s -zq. TRUE SIZE 2881 (0xb41)
   -- manifest says 337 (do not trust it; report only). Both jump tables emit
   correctly (6-entry type table + 43-entry subtype table = 49 leading obj
   fixups; compare code tail after 196B tables + pad; tools_diff_fn2.py does
   this with the true size). Structure, both loops, chain walk, all 9 case
   bodies, table shapes and case membership verified against a full objdump of
   the region (0x11e44..0x12833 hidden bytes recovered -- Ghidra's carving
   there is garbled but it IS code).

   METRIC NOTE: earlier sessions tuned for LCS (best LCS 1962/2881 at
   edit-dist ~1299, ours 3308B, `short rx`). THIS session tunes for _probe.sh
   EDIT-DIST: `volatile short rx` is now applied and cuts 1299 -> 1125. It
   fixes wall (1) (EDI home flips to xx, matching the target -- see below) at
   the cost of a yy/ryy ESI<->ECX reshuffle. That reshuffle raises LCS-loss
   but LOWERS edit-distance (the EDI fix realigns register bytes across every
   body; the reshuffle is just ~40 symmetric register-field substitutions).
   Net -174 edit-dist. Confirmed better than: volatile rx+ryy (1280), dropping
   the redundant case-1 node[0xb]&1 retest (1131), reordering the case-1 cmp
   (1141), hoisting one body's rx/2 to tip the tie non-volatile (1179).
   LENGTH vs MATCH tradeoff (2026-07-20, reloc code-only diff, table excluded):
   volatile rx = +234B length / 2568B diff; plain `short rx` = +219B / 2625B.
   short rx is 15B shorter but 57B worse on the match metric -> volatile kept.
   The +234 length residue is box-test spill churn: every body reloads xi/yi
   twice (target computes xi-hrx while xx is still fresh in EDX, before loading
   node+4; ours loads node+4 first, clobbering EDX, forcing the reloads). This
   is coupled to the yy/ryy recolor, not the C shape: forcing the sub early via
   a named lo local (+290), flipping the case-1 cmp operands (+248), and a
   volatile local copy of rx (+236) all REGRESS. Genuine plateau.
   WALLS (0x128b8 sibling family, coupled one-bit allocation choices):
   (1) EDI home: target xx->EDI with rx slot-read per use. xx and rx tie at
       19 reads each (6 direct bodies compute rx/2 TWICE, matching xx's two
       uses; target recomputes, does not hoist), so EDI is a genuine codegen
       tie our compile lost to rx and the target won for xx. RESOLVED for the
       edit-dist metric with `volatile short rx` (forces rx to memory -> xx
       wins EDI). Side effect: yy/ryy swap ESI<->ECX vs target (volatile
       removes rx from the interference graph, recoloring the siblings); a
       non-volatile tie-break that keeps yy=ESI/ryy=ECX was not found (tipping
       via a hoisted rx/2 diverges that body and nets worse). volatile z IS
       still load-bearing (keeps z un-homed).
   (2) frame 0x8c vs 0x84 and the ~33 spill-slot assignment order (sibling
       note: slot choice tracks neither declaration nor init order).
   (3) minor: chain-head node[0xb]&1 test CSEs into mov ah/test ah vs two
       direct test mem,1; gx-init reads xx before rx (target rx first);
       me-store scheduling.
   Levers already applied and load-bearing: full case coverage (case 0/3 +
   all 33 miss subtypes spelled out) to force the un-rebased jump tables
   (without them Watcom emits dec al + 5-entry table / compare tree);
   short params used directly (a1/xx/yy/ryy home EBP/ESI/ECX right);
   named dy for the entry guard sub-into-EDX; named per-body xi/n4/yi/n6/zi
   locals exactly where the target spills; named hrx/hry only in the five
   spill bodies (case1, 5.19, 5.1a, 5.1c, 5.26) so rx/2 is computed once
   there and recomputed inline elsewhere; id-then-i=0 order; goto negative
   guards with per-body cmp orientations transcribed from the target.

   @ 0x11d68: find blocking/hit entity near (x,y,z), TRUE SIZE 2881 bytes
   (0x11d68..0x128a8 inclusive; manifest badly undercounts at 337 -- the
   headless sweep truncated at the indirect jmp CS:[EAX*4+0x4550] @ 0x11e3c.
   That address is CODE: the old AGENTS note calling 0x11e3c a writable global
   predates the cont.19 0x28b8-extraction finding -- the errno_ptr literal
   0x11e3c is a RUNTIME address = manifest 0xf584 in the cut-off prefix).

   Scans grid cells gx = (x-rx)&0xff00 .. (x+rx)&0xff00, gy likewise with ry,
   walking each cell's g_grid_heads[((gy&0x7f00)>>1)|((gx>>8)&0x7f)] id chain
   (g_entity_pool node pool, link word +0, bound 0x400 nodes). Skips self and nodes
   with flag bit0 (+0xb). Dispatches on type byte +0x18 via a 6-entry jump
   table (obj1:+0x4550 = manifest 0x11c98): types 0,3 -> miss;
   type 1 (ped):    also +0xa bit0; if (self[0x1c]&2) same-squad skip
                    ((p-g_pool_a)/0x5c/8 equal); if self[0x1c]&0xc and node
                    +0x1c&0xc skip; skip if self is node's leader
                    (g_entity_pool + word(node+0x20)); box +-(rx/2 +0x20 slack),
                    +-(ry/2 +0x20), z in [node8-rz, node8+0x100].
   type 4:          subtype +0x19 must be 0xc, +0xa bit0 clear, box slack
                    0x40/0x40, z +0x100/rz.
   type 2 (vehicle): box slack 0x80, z +0x100/rz, skip if node's word +0x1c
                    == self's pool index (self - g_entity_pool).
   type 5 (static): nested 43-entry jump table (obj1:+0x4568 = manifest
                    0x11cb0) on subtype +0x19 (> 0x2a -> miss):
                    1 -> slack 0x40, z+0x100; 0xb -> 0x20, z+0x190;
                    0xc -> 0x80, z+0x180; 0x16 -> 0x40, z+0x170;
                    0x19 -> 0x40, z+0x100; 0x1a -> 0x40, z+0x100;
                    0x1c -> 0x80, z+0x100; 0x26/0x27/0x29 -> 0x80, z+0x100;
                    others -> miss.
   Returns the node pointer on overlap, else 0.
   Caller 0x2e808-family: FUN_11d68(p, x, y, z, 0x80, 0x80, 0x100). */
extern unsigned short g_grid_heads[];
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];

unsigned char *find_blocking_entity(unsigned char *a1, short xx, short yy, volatile short z,
                            volatile short rx, short ryy, short rz)
{
    int gx;
    int gy;
    int ix;
    unsigned char *me;
    unsigned short i;
    unsigned short id;
    unsigned char *node;
    unsigned char t;
    unsigned char st;
    unsigned char fl;
    unsigned char *q;
    int ga;
    int dy;
    int hrx;
    int hry;
    int xi, n4, yi, n6, zi;
    int xi5, n45, yi5, n65, n85;
    int xi6, n46, yi6, n66, zi6;
    int xi7, n47, yi7, n67, zi7;
    int xi8, n48, yi8, n68, zi8;

    dy = yy - ryy;
    if (dy <= 0)
        return 0;
    me = a1;
    for (gx = (xx - rx) & 0xff00; ((xx + rx) & 0xff00) >= gx; gx += 0x100) {
        dy = yy - ryy;
        gy = dy & 0xff00;
        ix = (gx >> 8) & 0x7f;
        for (; ((yy + ryy) & 0xff00) >= gy; gy += 0x100) {
            id = g_grid_heads[((gy & 0x7f00) >> 1) | ix];
            i = 0;
            if (id != 0) {
                do {
                    node = g_entity_pool + id;
                    if (node == me)
                        goto skip;
                    if ((unsigned char)(node[0xb] & 1))
                        goto skip;
                    t = node[0x18];
                    switch (t) {
                    case 0:
                    case 3:
                        goto skip;
                    case 1:
                        if ((unsigned char)(node[0xb] & 1))
                            goto skip;
                        if (node[0xa] & 1)
                            goto skip;
                        fl = a1[0x1c];
                        q = node;
                        if (fl & 2) {
                            ga = (int)(a1 - g_pool_a) / 0x5c / 8;
                            if ((int)(node - g_pool_a) / 0x5c / 8 == ga)
                                goto skip;
                        }
                        if (a1[0x1c] & 0xc) {
                            if (q[0x1c] & 0xc)
                                goto skip;
                        }
                        if (a1 == g_entity_pool + *(unsigned short *)(q + 0x20))
                            goto skip;
                        hrx = rx / 2;
                        xi = xx;
                        if (*(short *)(node + 4) + 0x20 < xi - hrx)
                            goto skip;
                        n4 = *(short *)(node + 4);
                        if (xi + hrx < n4 - 0x20)
                            goto skip;
                        hry = ryy / 2;
                        yi = yy;
                        if (*(short *)(node + 6) + 0x20 < yi - hry)
                            goto skip;
                        n6 = *(short *)(node + 6);
                        if (n6 - 0x20 > yi + hry)
                            goto skip;
                        zi = (short)z;
                        if (*(short *)(node + 8) + 0x100 < zi)
                            goto skip;
                        if (*(short *)(node + 8) > zi + rz)
                            goto skip;
                        return node;
                    case 4:
                        if (node[0x19] != 0xc)
                            goto skip;
                        if (node[0xa] & 1)
                            goto skip;
                        if (*(short *)(node + 4) + 0x40 < xx - rx / 2)
                            goto skip;
                        if (*(short *)(node + 4) - 0x40 > xx + rx / 2)
                            goto skip;
                        if (*(short *)(node + 6) + 0x40 < yy - ryy / 2)
                            goto skip;
                        if (*(short *)(node + 6) - 0x40 > yy + ryy / 2)
                            goto skip;
                        if (*(short *)(node + 8) + 0x100 < (short)z)
                            goto skip;
                        if ((short)z + rz < *(short *)(node + 8))
                            goto skip;
                        return node;
                    case 2:
                        if (*(short *)(node + 4) + 0x80 < xx - rx / 2)
                            goto skip;
                        if (*(short *)(node + 4) - 0x80 > xx + rx / 2)
                            goto skip;
                        if (*(short *)(node + 6) + 0x80 < yy - ryy / 2)
                            goto skip;
                        if (*(short *)(node + 6) - 0x80 > yy + ryy / 2)
                            goto skip;
                        if (*(short *)(node + 8) + 0x100 < (short)z)
                            goto skip;
                        if (*(short *)(node + 8) > rz + (short)z)
                            goto skip;
                        if (*(unsigned short *)(node + 0x1c) == (unsigned short)(a1 - g_entity_pool))
                            goto skip;
                        return node;
                    case 5:
                        st = node[0x19];
                        switch (st) {
                        case 0: case 2: case 3: case 4: case 5: case 6:
                        case 7: case 8: case 9: case 0xa: case 0xd: case 0xe:
                        case 0xf: case 0x10: case 0x11: case 0x12: case 0x13:
                        case 0x14: case 0x15: case 0x17: case 0x18: case 0x1b:
                        case 0x1d: case 0x1e: case 0x1f: case 0x20: case 0x21:
                        case 0x22: case 0x23: case 0x24: case 0x25: case 0x28:
                        case 0x2a:
                            goto skip;
                        case 1:
                            if (*(short *)(node + 4) + 0x40 < xx - rx / 2)
                                goto skip;
                            if (xx + rx / 2 < *(short *)(node + 4) - 0x40)
                                goto skip;
                            if (*(short *)(node + 6) + 0x40 < yy - ryy / 2)
                                goto skip;
                            if (*(short *)(node + 6) - 0x40 > yy + ryy / 2)
                                goto skip;
                            if (*(short *)(node + 8) + 0x100 < (short)z)
                                goto skip;
                            if (rz + (short)z < *(short *)(node + 8))
                                goto skip;
                            return node;
                        case 0xb:
                            if (*(short *)(node + 4) + 0x20 < xx - rx / 2)
                                goto skip;
                            if (xx + rx / 2 < *(short *)(node + 4) - 0x20)
                                goto skip;
                            if (*(short *)(node + 6) + 0x20 < yy - ryy / 2)
                                goto skip;
                            if (*(short *)(node + 6) - 0x20 > yy + ryy / 2)
                                goto skip;
                            if (*(short *)(node + 8) + 0x190 < (short)z)
                                goto skip;
                            if (rz + (short)z < *(short *)(node + 8))
                                goto skip;
                            return node;
                        case 0xc:
                            if (*(short *)(node + 4) + 0x80 < xx - rx / 2)
                                goto skip;
                            if (xx + rx / 2 < *(short *)(node + 4) - 0x80)
                                goto skip;
                            if (*(short *)(node + 6) + 0x80 < yy - ryy / 2)
                                goto skip;
                            if (*(short *)(node + 6) - 0x80 > yy + ryy / 2)
                                goto skip;
                            if (*(short *)(node + 8) + 0x180 < (short)z)
                                goto skip;
                            if (rz + (short)z < *(short *)(node + 8))
                                goto skip;
                            return node;
                        case 0x16:
                            if (xx - rx / 2 > *(short *)(node + 4) + 0x40)
                                goto skip;
                            if (*(short *)(node + 4) - 0x40 > xx + rx / 2)
                                goto skip;
                            if (*(short *)(node + 6) + 0x40 < yy - ryy / 2)
                                goto skip;
                            if (*(short *)(node + 6) - 0x40 > yy + ryy / 2)
                                goto skip;
                            if (*(short *)(node + 8) + 0x170 < (short)z)
                                goto skip;
                            if ((short)z + rz < *(short *)(node + 8))
                                goto skip;
                            return node;
                        case 0x19:
                            hrx = rx / 2;
                            xi5 = xx;
                            n45 = *(short *)(node + 4);
                            if (n45 + 0x40 < xi5 - hrx)
                                goto skip;
                            if (xi5 + hrx < n45 - 0x40)
                                goto skip;
                            hry = ryy / 2;
                            yi5 = yy;
                            n65 = *(short *)(node + 6);
                            if (n65 + 0x40 < yi5 - hry)
                                goto skip;
                            if (yi5 + hry < n65 - 0x40)
                                goto skip;
                            n85 = *(short *)(node + 8);
                            if ((short)z > n85 + 0x100)
                                goto skip;
                            if ((short)z + rz < n85)
                                goto skip;
                            return node;
                        case 0x1a:
                            hrx = rx / 2;
                            xi6 = xx;
                            n46 = *(short *)(node + 4);
                            if (n46 + 0x40 < xi6 - hrx)
                                goto skip;
                            if (xi6 + hrx < n46 - 0x40)
                                goto skip;
                            hry = ryy / 2;
                            yi6 = yy;
                            n66 = *(short *)(node + 6);
                            if (n66 + 0x40 < yi6 - hry)
                                goto skip;
                            if (yi6 + hry < n66 - 0x40)
                                goto skip;
                            zi6 = (short)z;
                            if (*(short *)(node + 8) + 0x100 < zi6)
                                goto skip;
                            if (*(short *)(node + 8) > zi6 + rz)
                                goto skip;
                            return node;
                        case 0x1c:
                            hrx = rx / 2;
                            xi7 = xx;
                            n47 = *(short *)(node + 4);
                            if (n47 + 0x80 < xi7 - hrx)
                                goto skip;
                            if (xi7 + hrx < n47 - 0x80)
                                goto skip;
                            hry = ryy / 2;
                            yi7 = yy;
                            n67 = *(short *)(node + 6);
                            if (n67 + 0x80 < yi7 - hry)
                                goto skip;
                            if (yi7 + hry < n67 - 0x80)
                                goto skip;
                            zi7 = (short)z;
                            if (*(short *)(node + 8) + 0x100 < zi7)
                                goto skip;
                            if (*(short *)(node + 8) > zi7 + rz)
                                goto skip;
                            return node;
                        case 0x26:
                        case 0x27:
                        case 0x29:
                            hrx = rx / 2;
                            xi8 = xx;
                            n48 = *(short *)(node + 4);
                            if (n48 + 0x80 < xi8 - hrx)
                                goto skip;
                            if (xi8 + hrx < n48 - 0x80)
                                goto skip;
                            hry = ryy / 2;
                            yi8 = yy;
                            n68 = *(short *)(node + 6);
                            if (n68 + 0x80 < yi8 - hry)
                                goto skip;
                            if (n68 - 0x80 > yi8 + hry)
                                goto skip;
                            zi8 = (short)z;
                            if (*(short *)(node + 8) + 0x100 < zi8)
                                goto skip;
                            if (*(short *)(node + 8) <= zi8 + rz)
                                return node;
                            goto skip;
                        }
                        goto skip;
                    }
                skip:
                    i++;
                    id = *(unsigned short *)node;
                    if (i >= 0x400)
                        break;
                } while (id != 0);
            }
        }
    }
    return 0;
}
