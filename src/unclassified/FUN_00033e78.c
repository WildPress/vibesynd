/* PARKED near-miss (NOT matched) -- 311B vs target 310B; TRUE size 310 (manifest
 * says 150, undercounted: carve stops at the jmp CS:[...] -- code runs to the
 * ret at 0x33fad). Entry block, tile lookup tail, switch, and ALL EIGHT case
 * bodies are byte-identical; the residue is one block at obj 0x31..0x54:
 * row homed EDI vs target ECX (3 x 1-byte reg digits) and the slot formation
 * order -- ours `lea eax,[eax*4]; mov ecx,[g_5358](6B); movsx edi,z` vs target
 * `movsx edi,z; lea ecx,[eax*4]; mov eax,[g_5358](A1,5B)` (the +1 byte).
 * LEVERS THAT WORKED (kept): (1) ONE int local xs holds the 0x6000 modulo
 * divisor AND then x -- the call-crossing web forces ESI, giving the target's
 * be/f7fe divisor and clean `push esi` case bodies (with int-param helper
 * protos; short protos re-derive via movsx eax,cx); (2) volatile on the
 * g_5358 pointer decl keeps its load out of the far-hoist slot. WALL: the
 * row->ECX / scaled->lea-ECX / base->A1-EAX triangle is allocator-internal;
 * int-cast slot spellings (scaled + (int)g_5358) flip lea to ECX but break
 * the tile staging (xor edx/mov dl) and re-home xs -- same register-role
 * family as the parked twins FUN_00033b88/FUN_00033db8 (playbook 3).
 *
 * @ 0x33e78 (jump-table dispatcher, 6-entry table at manifest 0x33e60, jmp
 * literal 0x26718). Map-tile hit dispatcher: same map/tile lookup as
 * FUN_00033fb8 (row = (y % 0x6000)/256, col = (x & 0xff00)/256, slot =
 * g_5358 + col + row*128, tile byte at (z-1)/128 + *slot) but instead of a
 * passability bool it dispatches on the tile class g_10ac0[tile]:
 *   6 -> FUN_00033b88(x,y,z)   7 -> FUN_00033c38(x,y,z)
 *   8 -> FUN_00033cf8(x,y,z)   9 -> FUN_00033db8(x,y,z)
 *   0xb -> try all four in that order, 1 on first hit
 *   0xa / anything else -> 0
 * Case map (table at 0x33e60, index = g_10ac0[tile] - 6, ja > 5 -> default):
 *   idx0->0x33f08 idx1->0x33f17 idx2->0x33f26 idx3->0x33f35
 *   idx4(0xa)->0x33fa8(default) idx5(0xb)->0x33f44.
 * Ghidra mis-decodes the case heads: each true body starts one byte earlier with
 * PUSH EDI (0x57) at 0x33f08/0x33f17/0x33f26/0x33f35/0x33f44.
 */
extern char **volatile g_5358;
extern unsigned char *g_10ac0;

int FUN_00033b88(int x, int y, int z);
int FUN_00033c38(int x, int y, int z);
int FUN_00033cf8(int x, int y, int z);
int FUN_00033db8(int x, int y, int z);

unsigned short FUN_00033e78(short x, short y, short z)
{
    unsigned short r;
    int xs;
    int row;
    int col;
    int index;
    unsigned char tile;

    xs = 0x6000;
    row = (y % xs) / 256;
    xs = x;
    col = (xs & 0xff00) / 256;
    index = col + row * 128;
    tile = *(unsigned char *)((z - 1) / 128 + (int)*(g_5358 + index));
    switch (g_10ac0[tile]) {
    case 6:
        return FUN_00033b88(xs, y, z);
    case 7:
        return FUN_00033c38(xs, y, z);
    case 8:
        return FUN_00033cf8(xs, y, z);
    case 9:
        return FUN_00033db8(xs, y, z);
    case 0xb:
        r = FUN_00033b88(xs, y, z);
        if (r != 0)
            return 1;
        r = FUN_00033c38(xs, y, z);
        if (r != 0)
            return 1;
        r = FUN_00033cf8(xs, y, z);
        if (r != 0)
            return 1;
        r = FUN_00033db8(xs, y, z);
        if (r != 0)
            return 1;
        return r;
    }
    return 0;
}
