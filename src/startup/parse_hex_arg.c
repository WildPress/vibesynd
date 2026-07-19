/* parse_hex_arg @ 0x24b08 (221 bytes) -- parse a hex-digit string into a packed
 * nibble mask. One stack param (char *s at [ESP+0x14] after push ebx/esi/edi/ebp).
 *
 * What it does:
 *   len = strlen(s);                       // inlined repne-scasb intrinsic
 *   if (len == 0) return 0;
 *   mask = 0; bitpos = 0;
 *   for (i = len-1; i >= 0; i--, bitpos += 4) {   // right-to-left, one nibble/char
 *       c = s[i] - '0';                    // fold digits and 'A'..'F' into 0..0x16
 *       switch (c) {                       // dense 23-entry jump table
 *         case 0..9   -> v = 0..9;
 *         case 0x11..0x16 -> v = 0xa..0xf; // 'A'..'F'
 *         default (0xa..0x10, and c>0x16)  -> skip (':'..'@' and non-hex)
 *       }
 *       mask |= v << bitpos;               // place nibble at its hex position
 *   }
 *   return mask;                           // in EAX (EDX holds leftover bitpos)
 *
 * The switch compiles to `and ecx,0xff; jmp cs:[ecx*4+0x17360]` -- exactly the
 * target's dispatch. len/i/bitpos are 16-bit (short): the target uses test cx,cx,
 * test bx,bx, and movsx ecx,dx for the shift count, so those widths are load-bearing.
 *
 * RECONSTRUCTED from a broken 99-byte stub (was 16% / dist 186). The whole code
 * body (0x24b08..0x24be4) is now byte-identical to the target EXCEPT a stable
 * register-naming tie: the target holds mask in EAX and bitpos in EDX (temp v in
 * ESI), whereas our build holds mask in EDX and bitpos in ESI (temp v in EAX);
 * no source reordering flips Watcom's colouring. Reported dist 140 is dominated by
 * the 92-byte inline jump table (+4-byte align pad) that Watcom co-locates in our
 * single-function object .text: in the shipped binary that table lives in a far CS
 * pool (CS:[...+0x17360]) outside the function window, so match_reloc counts it as
 * pure insertion. That co-location is a build-isolation artefact, not a source bug.
 */
#include <string.h>

unsigned parse_hex_arg(char *s)
{
    unsigned short len;
    short i;
    unsigned mask;
    short bitpos;
    unsigned char c;
    unsigned v;

    len = (unsigned short)strlen(s);
    if (len == 0)
        return 0;

    mask = 0;
    bitpos = 0;
    for (i = (short)(len - 1); i >= 0; i--, bitpos += 4) {
        c = (unsigned char)(s[i] - 0x30);
        switch (c) {
        case 0:    v = 0;   break;
        case 1:    v = 1;   break;
        case 2:    v = 2;   break;
        case 3:    v = 3;   break;
        case 4:    v = 4;   break;
        case 5:    v = 5;   break;
        case 6:    v = 6;   break;
        case 7:    v = 7;   break;
        case 8:    v = 8;   break;
        case 9:    v = 9;   break;
        case 0x11: v = 0xa; break;
        case 0x12: v = 0xb; break;
        case 0x13: v = 0xc; break;
        case 0x14: v = 0xd; break;
        case 0x15: v = 0xe; break;
        case 0x16: v = 0xf; break;
        default:   continue;
        }
        mask |= v << bitpos;
    }
    return mask;
}
