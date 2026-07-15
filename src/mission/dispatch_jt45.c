/* NEAR-MISS (parked) @ 0x1a458 -- jump-table dispatcher, 45-entry table at
 * manifest 0x1a39c = jmp CS:[edx*4+0xcc54]; table precedes the fn, 8B gap.
 * TRUE SIZE 845 (0x34d): 0x1a458..0x1a7a4 incl (manifest says 70 -- badly
 * undercounted; compare window truncates). Recipe: -4s -oneatx -zp8 -s -zq.
 *
 * STATE (ours 778 code bytes vs 845): head, body A arm1 (incl 753f jnz),
 * A arm2 (minus its 3-byte lea, see below), B both arms, C/D/E bodies,
 * G body, all four constant bodies, I and L 100%, N head + N arm2 100%,
 * and the tail-compare logic all byte-match (masked). Remaining diffs:
 *  1. F (cases 0x12..0x16): Watcom cross-jump MERGES our two value-identical
 *     arms into one (deletes the jnz + 2nd arm, -26B). goto-restructure,
 *     volatile, and arm-spelling swaps all failed to keep both arms. WALL:
 *     the arms are semantically equal; every spelling converges.
 *  2. A2/N1 lea order: our lea ecx,[edx+eax] (8d0c02) == N2's, so it
 *     cross-jumps away; target A2 has its own 8d0c10 ([eax+edx]). N1 also
 *     flips to the SIB-merge shape (A1's identical spelling accumulates
 *     correctly -- body-adjacent-to-merge-host context).
 *  3. compare-temp register rotor: B/C/E/G temps land SI/CX/DX/CX where the
 *     target has DI/SI/CX/AX (I/L/N land right). Register-role tie-break
 *     class; G's copy-vs-reload (6689c8 vs 31c0 668b430e) is downstream.
 *  4. tail g_frame_sprite[frame]: target materialises lea ecx,[edx*2+0x0] then
 *     [ecx+edx]; ours SIB-folds [ecx+edx*2] (scaled-lea wall, cf 0x18ae8).
 * KEY LEVERS FOUND: `if (x != 1) goto Lelse;` keeps the 75 jnz AND gives the
 * fall-through block else-position codegen (accumulate shape); physical arm1
 * of a plain if/else ALWAYS takes the merge/lea shape (arm2 the accumulate) --
 * a positional personality no spelling flips. Sum operand order picks the
 * accumulator: `tbl[d] + anim` -> anim in ECX + and-form widen of tbl (target
 * A1/N1); `anim + tbl[d]` -> tbl in ECX SIB-folded (target B). Bytecast
 * `*(ushort*)(dtype*2 + (char*)tbl)` -> SIB load into DX + lea (target A2/N2).
 *
 * Sprite-frame selector: switch on entity type b[0x19] (0..0x2c), compute a
 * frame number from anim state word b+0xe, facing byte b+0x1a and (for armed
 * types) the equipped item's type (node = g_entity_pool + word b+0x44, guarded
 * against the pool bound g_11670); then if frame != cached word b+0x12,
 * store sprite g_frame_sprite[frame] to b+0x10 and cache frame at b+0x12.
 *
 * Case map (type -> body):
 *   0,1,6,7,8,0xb,0xc,0xd,0x10,0x1c,0x22,0x24,0x26,0x28,0x29 -> A (tables a5e6/a546)
 *   2,3,4,5,0xe,0xf,0x1d,0x1e,0x1f,0x20,0x25,0x27           -> B (tables a60e/a56e)
 *   9,0xa -> anim+0xc0        0x14,0x17 -> ((dir+0x20)/64&1)+0xd0
 *   0x11  -> anim+0xc5        0x12,0x13,0x15,0x16 -> anim+0xc1+((dir+0xa0)/64&3)
 *   0x18  -> anim==1 ? anim+0xcb+((dir+0x20)/64&1)*2 : anim+0xcd
 *   0x19  -> 0xd2             0x1a -> anim==1 ? ((dir+0x20)/64&1)*2+anim+0xcc : anim+0xce
 *   0x1b  -> 0xd3             0x21 -> anim+0xc9      0x23 -> 0x33c
 *   0x2b  -> N (tables a636/a596)                    0x2c -> 0xec
 *   0x2a, default -> frame unchanged (uninitialised)
 */
extern unsigned char g_entity_pool[];
extern unsigned char g_11670;
extern unsigned short *g_frame_sprite;
extern unsigned short g_a546[];
extern unsigned short g_a56e[];
extern unsigned short g_a596[];
extern unsigned short g_a5e6[];
extern unsigned short g_a60e[];
extern unsigned short g_a636[];

void dispatch_jt45(unsigned char *b)
{
    unsigned short frame;

    switch (b[0x19]) {
    case 0: case 1: case 6: case 7: case 8: case 0xb: case 0xc: case 0xd:
    case 0x10: case 0x1c: case 0x22: case 0x24: case 0x26: case 0x28:
    case 0x29:
    {
        unsigned short anim = *(unsigned short *)(b + 0xe);
        unsigned short dtype = 0;
        if (anim != 1)
            goto Aelse;
        {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = g_a5e6[dtype] + *(unsigned short *)(b + 0xe)
                  + ((b[0x1a] + 0x10) / 32 & 7);
        }
        break;
    Aelse:
        {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = *(unsigned short *)(dtype * 2 + (char *)g_a546)
                  + *(unsigned short *)(b + 0xe)
                  + ((b[0x1a] + 0x10) / 32 & 7);
        }
        break;
    }
    case 2: case 3: case 4: case 5: case 0xe: case 0xf: case 0x1d:
    case 0x1e: case 0x1f: case 0x20: case 0x25: case 0x27:
    {
        unsigned short dtype = 0;
        if (*(unsigned short *)(b + 0xe) == 1) {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = *(unsigned short *)(b + 0xe) + g_a60e[dtype]
                  + ((b[0x1a] + 0x10) / 32 & 7);
        } else {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = *(unsigned short *)(b + 0xe) + g_a56e[dtype]
                  + ((b[0x1a] + 0x10) / 32 & 7);
        }
        break;
    }
    case 9: case 0xa:
        if (*(unsigned short *)(b + 0xe) == 1)
            frame = *(unsigned short *)(b + 0xe) + 0xc0;
        else
            frame = *(unsigned short *)(b + 0xe) + 0xc0;
        break;
    case 0x14: case 0x17:
        frame = ((b[0x1a] + 0x20) / 64 & 1) + 0xd0;
        break;
    case 0x11:
        if (*(unsigned short *)(b + 0xe) == 1)
            frame = *(unsigned short *)(b + 0xe) + 0xc5;
        else
            frame = *(unsigned short *)(b + 0xe) + 0xc5;
        break;
    case 0x12: case 0x13: case 0x15: case 0x16:
        if (*(unsigned short *)(b + 0xe) != 1)
            goto Felse;
        frame = ((b[0x1a] + 0xa0) / 64 & 3)
              + (*(volatile unsigned short *)(b + 0xe) + 0xc1);
        break;
    Felse:
        frame = *(unsigned short *)(b + 0xe) + 0xc1
              + ((b[0x1a] + 0xa0) / 64 & 3);
        break;
    case 0x18:
    {
        unsigned short a = *(unsigned short *)(b + 0xe);
        if (a != 1)
            goto Gelse;
        frame = *(volatile unsigned short *)(b + 0xe) + 0xcb
              + ((b[0x1a] + 0x20) / 64 & 1) * 2;
        break;
    Gelse:
        frame = a + 0xcd;
        break;
    }
    case 0x19:
        frame = 0xd2;
        break;
    case 0x1a:
        if (*(unsigned short *)(b + 0xe) == 1)
            frame = ((b[0x1a] + 0x20) / 64 & 1) * 2
                  + (*(unsigned short *)(b + 0xe) + 0xcc);
        else
            frame = *(unsigned short *)(b + 0xe) + 0xce;
        break;
    case 0x1b:
        frame = 0xd3;
        break;
    case 0x23:
        frame = 0x33c;
        break;
    case 0x21:
        if (*(unsigned short *)(b + 0xe) == 1)
            frame = *(unsigned short *)(b + 0xe) + 0xc9;
        else
            frame = *(unsigned short *)(b + 0xe) + 0xc9;
        break;
    case 0x2c:
        frame = 0xec;
        break;
    case 0x2b:
    {
        unsigned short dtype = 0;
        if (*(unsigned short *)(b + 0xe) != 1)
            goto Nelse;
        {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = g_a636[dtype] + *(unsigned short *)(b + 0xe)
                  + ((b[0x1a] + 0x10) / 32 & 7);
        }
        break;
    Nelse:
        {
            unsigned char *node = g_entity_pool + *(unsigned short *)(b + 0x44);
            if (node >= &g_11670)
                dtype = node[0x19];
            frame = *(unsigned short *)(b + 0xe)
                  + *(unsigned short *)((char *)g_a596 + dtype * 2)
                  + ((b[0x1a] + 0x10) / 32 & 7);
        }
        break;
    }
    case 0x2a:
        break;
    }
    if (frame != *(unsigned short *)(b + 0x12)) {
        *(unsigned short *)(b + 0x10) = g_frame_sprite[frame];
        *(unsigned short *)(b + 0x12) = frame;
    }
}
