/* WALL x3 (repne-scasb strlen intrinsic + inline jump-table dispatch + 64-bit
 *          return type Watcom 9.5b cannot express) @ 0x24b08
 *
 * Ground truth (disassemble_function 0x24b08). One stack param (char *s at
 * [ESP+0x14] after push ebx/esi/edi/ebp); Ghidra's __fastcall/3-param sig is
 * phantom. Returns a 64-bit mask in EDX:EAX. Structure:
 *
 *   edi = s;
 *   ecx = -1; al = 0; repne scasb; ecx = ~ecx; ecx--;     // ecx = strlen(s)
 *   if ((short)ecx == 0) return 0;                          // empty -> 0
 *   ebx = ecx - 1;                                          // index, right->left
 *   eax = 0; edx = 0;                                       // mask lo, bit position
 *   for (; (short)ebx >= 0; ebx--, edx += 4) {              // 4 bits (a nibble) per char
 *       cl = s[ebx] - 0x30;                                 // 'c' - '0'
 *       if ((unsigned char)cl > 0x16) continue;             // out of range -> skip
 *       switch (cl) {                  // JMP CS:[ecx*4 + 0x17360], 23 entries
 *         case <hit>: esi = 1; esi <<= (edx & 0xff); eax |= esi;  // set bit at position edx
 *         default:    ;                                     // (only ONE inlined case body)
 *       }
 *   }
 *   return edx:eax;
 *
 * WHY THIS CANNOT REACH A MASKED 'YES' (two independent, source-unreachable walls):
 *
 *  1) repne-scasb strlen intrinsic (playbook Section 3, cf. 0x17998/0x299c8):
 *     the target inlines strlen as `sub ecx,ecx; dec ecx; xor eax,eax; repne
 *     scasb; not ecx; dec ecx`. Our batch compile has no <string.h> on the
 *     include path and neither -oi nor #pragma intrinsic triggers the inline
 *     for strlen; any C spelling emits a byte-compare loop instead, diverging
 *     from prologue byte ~5 onward.
 *
 *  2) inline jump-table dispatch (playbook Section 0, cf. 0x23038): the 23-entry
 *     switch table lives in a FAR segment in the shipped binary (CS:[ecx*4 +
 *     0x17360]) so the on-disk function body is clean, but Watcom co-locates the
 *     table (+ alignment pad) inside our object .text before the code. match_reloc
 *     compares the whole object text against the target window, so len(ours) can
 *     never equal len(target) even were the code byte-exact.
 *
 *  3) 64-bit return (playbook Section 3, cf. 0x39495): the mask is accumulated
 *     in EDX:EAX with a bit position (edx) that grows by 4 per char, i.e. a true
 *     64-bit result. Watcom 9.5b rejects `long long`/`__int64` (E1009/E1060), so
 *     the function's return width itself is not source-expressible. The
 *     reconstruction below narrows it to `unsigned` merely so the unit compiles.
 *
 *  MANIFEST SIZE UNDER-COUNTED: recorded size=98 truncates at the first case
 *  body; true extent is 0x24b08..0x24be4 = 221 bytes (0xdd). (headless pass
 *  stopped at the switch JMP.)
 *
 * Parked as WALL; not source-reachable. Reconstruction below is structural only.
 */
unsigned FUN_00024b08(char *s)
{
    unsigned len;
    int i;
    unsigned mask;              /* true target: 64-bit EDX:EAX (see WALL 3) */
    int bitpos;
    unsigned char c;

    for (len = 0; s[len]; len++)          /* target: repne scasb (WALL 1) */
        ;
    if ((short)len == 0)
        return 0;

    mask = 0;
    bitpos = 0;
    for (i = (int)len - 1; (short)i >= 0; i--, bitpos += 4) {
        c = (unsigned char)(s[i] - 0x30);
        if (c > 0x16)
            continue;
        switch (c) {                      /* target: JMP CS:[c*4+tbl] (WALL 2) */
        case 0:
            mask |= (unsigned)1 << (bitpos & 0xff);
            break;
        default:
            break;
        }
    }
    return mask;
}
