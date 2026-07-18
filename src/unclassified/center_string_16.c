/* frameless @ 0x299c8: center a string into a 16-char field padded with spaces.
   len = strlen(src); pad = (0x10 - len)/2. For each of 16 output positions i: emit a
   space if i < pad or i >= pad+len, else copy the next src char.

   MATCHED (96/96 bytes). Three moves cracked the old strlen-intrinsic wall:
   (1) #include <string.h> so #pragma intrinsic(strlen) inlines it as `repne scasb`
       (SUB ECX,ECX;DEC ECX;XOR EAX,EAX;REPNE SCASB;NOT ECX;DEC ECX) instead of a call.
       REQUIRES the Watcom headers on the include path -- see toolchain/README (copy the
       10.0a H/ into watcom95/H/; the minimal hand-prototype does NOT satisfy the 9.5
       intrinsic recogniser, the real header signatures do).
   (2) precompute `end = pad + len` so len dies early -> no register spill.
   (3) `i >= end` (not `end <= i`) -> the target's cmp-operand order. */
#include <string.h>
void center_string_16(char *dst, char *src)
{
    short len = strlen(src);
    short pad = (short)((0x10 - len) / 2);
    short end = pad + len;
    short i = 0;
    do {
        if (i < pad || i >= end)
            *dst = ' ';
        else
            *dst = *src++;
        i++;
        dst++;
    } while (i < 0x10);
}
