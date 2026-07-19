/* C runtime helper @ 0x3e361 (CLIB3S): path-separator normalize (mislabeled canon_path_sep). */
char canon_path_sep(char c, char *p)
{
    if (c == 92 || c == 47) {
        if (*p == 0)
            *p = c;
        c = *p;
    }
    return c;
}
