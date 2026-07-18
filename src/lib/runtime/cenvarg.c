/* C runtime helper @ 0x3dfcf (CLIB3S): string copy incl NUL (heuristic mislabeled 'cenvarg'). */
void cenvarg(char *dst, char *src)
{
    while ((*dst = *src) != 0) {
        src++;
        dst++;
    }
}
