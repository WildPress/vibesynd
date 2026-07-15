/* frameless @ 0x299c8: center a string into a 16-char field padded with spaces.
   len = strlen(src); pad = (0x10 - len)/2. For each of 16 output positions i: emit a
   space if i < pad or i >= pad+len, else copy the next src char.

   PARKED near-miss (NOT matched). The target inlines strlen as `repne scasb`
   (SUB ECX,ECX; DEC ECX; XOR EAX,EAX; REPNE SCASB; NOT ECX; DEC ECX). In our
   batch-compile setup strlen will NOT inline: -oi and #pragma intrinsic(strlen) both
   still emit a CALL, and <string.h> isn't on the include path (compile fails). So we
   emit `push; call strlen; add esp,4` (110B) vs the target's inline (96B). Would need
   the Watcom headers / intrinsic wiring in the container to match. Logic is correct. */
extern unsigned strlen(const char *);
void center_string_16(char *dst, char *src)
{
    short len = strlen(src);
    short pad = (short)((0x10 - len) / 2);
    short i = 0;
    do {
        if (i < pad || pad + len <= i)
            *dst = ' ';
        else
            *dst = *src++;
        i++;
        dst++;
    } while (i < 0x10);
}
