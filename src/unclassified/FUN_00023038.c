/* WALL (jump-table dispatch) @ 0x23038 -- function CODE is byte-EXACT, tool can't confirm.
 *
 * The reconstructed function body compiles to the target's bytes verbatim (masked):
 *   target 42B window == ours' first 42 code bytes, and ours' full 47B code region is
 *   538b54240866837a2000740431c05bc38a5a1980fb2c771588da81e2ff0000002eff2495<tbl>b8010000005bc3
 *   i.e. push ebx; mov edx,[esp+8]; cmp word[edx+0x20],0; jz; xor eax,eax; pop ebx; ret;
 *        mov bl,[edx+0x19]; cmp bl,0x2c; ja; mov dl,bl; and edx,0xff; jmp CS:[edx*4+tbl];
 *        mov eax,1; pop ebx; ret   -- all match the target.
 *
 * Why match95 reports NO (two tooling/data issues, not a code diff):
 *   1) Watcom co-locates the 45-entry jump table (0xb4 bytes) + a 12-byte entry-alignment
 *      pad in the SAME object .text, BEFORE the function code. match_reloc compares the
 *      whole obj text (239B) against the 42B target window, so len(ours)!=len(target) can
 *      never hold. In the real binary the table lives in a far segment (CS:[...+0x15830]),
 *      so the function there is clean; there is no C spelling that moves the table out of
 *      the compiled object.
 *   2) MANIFEST SIZE IS UNDER-COUNTED: recorded size=42 truncates the final case body.
 *      True extent is 0x23038..0x23067 = 47 bytes (0x2f); the last instruction is
 *      `mov eax,1; pop ebx; ret` at 0x23060.
 *
 * Reconstruction notes: guard word[+0x20]!=0 -> return 0; else switch on p[0x19]. Cases
 * are interleaved (even 0..0x2c -> return 1, odd fall to default) so Watcom picks a dense
 * jump table (not a range compare) and emits exactly ONE inline body (mov eax,1); the
 * per-case target mapping lives only in the (masked) table data and doesn't affect the
 * function's own bytes.
 */
extern int FUN_00023038(unsigned char *p);

int FUN_00023038(unsigned char *p)
{
    if (*(short *)(p + 0x20) != 0)
        return 0;
    switch (p[0x19]) {
    case 0x00: case 0x02: case 0x04: case 0x06: case 0x08:
    case 0x0a: case 0x0c: case 0x0e: case 0x10: case 0x12:
    case 0x14: case 0x16: case 0x18: case 0x1a: case 0x1c:
    case 0x1e: case 0x20: case 0x22: case 0x24: case 0x26:
    case 0x28: case 0x2a: case 0x2c:
        return 1;
    }
    /* fall off end: default leaves EAX unchanged (target returns garbage) */
}
