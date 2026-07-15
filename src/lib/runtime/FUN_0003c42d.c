/* @ 0x3c42d -- strupr: uppercase each char of s via toupper (0x3dce5). Recipe: -3s -d2 -os. */
extern int FUN_0003dce5(int c);
void FUN_0003c42d(char *s)
{
    while (*s != 0) {
        *s = (char)FUN_0003dce5((unsigned char)*s);
        s++;
    }
}
