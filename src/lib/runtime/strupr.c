/* @ 0x3c42d -- strupr: uppercase each char of s via toupper (0x3dce5). Recipe: -3s -d2 -os. */
extern int toupper(int c);
void strupr(char *s)
{
    while (*s != 0) {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}
