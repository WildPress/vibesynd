/* frameless @ 0x12da8: build + apply player k's equip-template row. TRUE SIZE
   1188 (0x4a4, 0x12da8..0x1324b incl.) -- manifest says 955: it truncates at
   the 4-way jump table `jmp CS:[EAX*4+0x5650]` (table file offset 0x12d98,
   entries 0x13095/0x130d2/0x1310c/0x13146 recovered via lefix.py +
   disassemble_bytes; 235 hidden bytes of case bodies + the shared 0x1317e
   tail). Called by matched FUN_00012ca8 (session init) for every remote
   player k with rec flag 2.

   🅿️ PARKED ~1090/1188 (9 source configs, -4s -oneatx -zp8 -s -zq; verify
   against TRUE size 0x4a4, not the manifest's 955 -- match95/match_reloc will
   mis-split the jump-table tail until the manifest size is corrected).
   MATCHED: whole entry (slot scan, code=k*8, row header + g_e8a7=1 via the
   reg-form store), the 18-entry clear loop incl. c@[esp]/i@[esp+4] slots and
   alignment pads, outer-loop guard/tail, best-research scan shape, the
   1beae/1beaf blocks' shapes incl. reg-form constant stores (the `int v, w`
   reused-named-constant lever gives target's `mov edx,0xc; mov ebx,0xc7;
   mov [..],dx` forms; plain literals fold to `66 c7` imm16 stores, 15B short),
   all four jump-table case bodies, the g_c368 20B record block byte-for-byte
   (incl. `and eax,0xff` inline widen, imul 0x5c, mov ebx,0x8110 form), and
   the FUN_223c8 call tail.
   WALL (allocator, spill-slot family like 0x338d8): the pair-index `slot`
   local homes in AL and SPILLS to a third stack slot ([esp+8], frame
   sub esp,0xc vs target 8, param disp 0x1c vs 0x18 rippling ~10 sites);
   target keeps it in CL across the two ifs. Fresh block var, reuse of the
   dead `code` var, and a pre-read copy all spill identically. Secondary
   role rotations riding on it: g_1beb1 load hoists to loop-body top (CH,
   target loads mid-block), v=0x10 lands EDX vs EBX, scan-loop best/lvl byte
   regs rotated, 1beaf's 0x417k chain in EDI vs EBX, switch index via BL vs
   BH, ch=1 vs al=1 at the c358 store, movzx/mov order in the else-arm pair
   store. Byte-count without the spill would be 1187-1191 vs 1188.

   Finds k's first active pool-A slot (bit 1 of +0x1c, base 0x8110, agent
   stride 0x2e0 = 8 slots x 0x5c) -> row header code k*8+slot in g_agent_slots
   (template row stride 0x417, cf. FUN_000223c8's docs); row word +0x356
   (g_e8a7) = 1; clears the 18 template entry slot-no bytes (g_squad_slot, entry
   stride 40). Marks g_c358[2k]=1 / g_c359[2k]=0. Then for each i < g_3eb5
   whose walking slot has flag 2: slot |= 0x10 at +0x1d, entry i gets
   slot-no i+1, word0 (g_e5ba) = 0x10, pair0 = (kind, count=g_a73a[kind])
   where kind = g_1beb1 if set else the highest-level researched kind
   (g_3ec0/g_3ec1 pair table, bank status word g_7c05[j*0x1f5] == 0x960);
   optional pair1 = (0xc, 0xc7) if g_1beae; optional next pair = (1, 0x32)
   if g_1beaf; ammo word (g_e5bc) = {0,0xaaa,0x1554,0x1ffe}[g_3eb7] via the
   jump table; and fills the 20-byte record at g_c368 + 20*(8k+i): 0xff,
   g_1beb0, k at +7, zeros, +0xc = pool-A slot ptr g_pool_a+(code+i)*0x5c,
   +0x10 = 0. Finally FUN_000223c8(k, 0x1002) applies the row. */
extern unsigned char g_pool_a[];
extern unsigned char g_agent_slots[];
extern unsigned char g_e8a7[];
extern unsigned char g_e5ba[];
extern unsigned char g_e5bc[];
extern unsigned char g_squad_slot[];
extern unsigned char g_equip_qty[];
extern unsigned char g_equip_kind[];
extern unsigned char g_c358[];
extern unsigned char g_c359[];
extern unsigned char g_c368[];
extern unsigned char g_3eb5;
extern unsigned char g_3eb7;
extern unsigned char g_3ec0[];
extern unsigned char g_3ec1[];
extern unsigned short g_a73a[];
extern unsigned char g_1beae;
extern unsigned char g_1beaf;
extern unsigned char g_1beb0;
extern unsigned char g_1beb1;
extern unsigned char g_7c05[];
extern void FUN_000223c8(unsigned short row, unsigned short mode);

void FUN_00012da8(unsigned char k)
{
    unsigned char c, i;
    unsigned char code;
    unsigned char *rec;
    int v, w;

    rec = g_pool_a + k * 0x2e0;
    code = k * 8;
    while (!(rec[0x1c] & 2)) {
        rec += 0x5c;
        code++;
    }
    g_agent_slots[k * 0x417] = code;
    v = 1;
    *(unsigned short *)(g_e8a7 + k * 0x417) = v;
    for (c = 0; c < 18; c++)
        g_squad_slot[k * 0x417 + c * 40] = 0;
    g_c359[k * 2] = 0;
    g_c358[k * 2] = 1;
    for (i = 0; i < g_3eb5; i++) {
        if (rec[0x1c] & 2) {
            rec[0x1d] |= 0x10;
            g_squad_slot[k * 0x417 + i * 40] = i + 1;
            v = 0x10;
            *(unsigned short *)(g_e5ba + k * 0x417 + i * 40) = v;
            if (g_1beb1 == 0) {
                unsigned char best, besti, j, lvl;
                best = 0;
                besti = 0;
                j = 0;
                for (;;) {
                    if (g_3ec1[j * 2] == 0)
                        break;
                    lvl = g_3ec0[j * 2];
                    if (lvl != 0) {
                        if (*(unsigned short *)(g_7c05 + j * 0x1f5) == 0x960) {
                            if (best < lvl) {
                                besti = j;
                                best = lvl;
                            }
                        }
                    }
                    j++;
                }
                {
                    unsigned short t = g_3ec1[besti * 2];
                    *(unsigned short *)(g_equip_kind + k * 0x417 + i * 40) = t;
                    *(unsigned short *)(g_equip_qty + k * 0x417 + i * 40) =
                        g_a73a[g_3ec1[besti * 2]];
                }
            } else {
                *(unsigned short *)(g_equip_kind + k * 0x417 + i * 40) = g_1beb1;
                *(unsigned short *)(g_equip_qty + k * 0x417 + i * 40) =
                    g_a73a[g_1beb1];
            }
            {
            unsigned char slot = 1;
            if (g_1beae != 0) {
                unsigned int e = k * 0x417 + i * 40 + 4;
                v = 0xc;
                w = 0xc7;
                *(unsigned short *)(g_equip_kind + e) = v;
                slot = 2;
                *(unsigned short *)(g_equip_qty + e) = w;
            }
            if (g_1beaf != 0) {
                v = 0x32;
                w = 1;
                *(unsigned short *)(g_equip_qty + k * 0x417 + i * 40 + slot * 4) = v;
                *(unsigned short *)(g_equip_kind + k * 0x417 + i * 40 + slot * 4) = w;
            }
            }
            switch (g_3eb7) {
            case 0:
                *(unsigned short *)(g_e5bc + k * 0x417 + i * 40) = 0;
                break;
            case 1:
                *(unsigned short *)(g_e5bc + k * 0x417 + i * 40) = 0xaaa;
                break;
            case 2:
                *(unsigned short *)(g_e5bc + k * 0x417 + i * 40) = 0x1554;
                break;
            case 3:
                *(unsigned short *)(g_e5bc + k * 0x417 + i * 40) = 0x1ffe;
                break;
            }
            {
                unsigned char *q = g_c368 + (k * 8 + i) * 20;
                q[0] = 0xff;
                q[1] = g_1beb0;
                q[2] = 0;
                q[3] = 0;
                q[4] = 0;
                q[5] = 0;
                q[6] = 0;
                q[7] = k;
                q[8] = 0;
                *(unsigned short *)(q + 0xa) = 0;
                *(unsigned char **)(q + 0xc) =
                    g_pool_a + (g_agent_slots[k * 0x417] + i) * 0x5c;
                *(int *)(q + 0x10) = 0;
            }
        }
        rec += 0x5c;
    }
    FUN_000223c8((unsigned short)k, 0x1002);
}
