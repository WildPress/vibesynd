char *FUN_0003e7f7(const char *s, int c)
{
    int ch = c;

    while ((unsigned char)*s != ch) {
        if (*s++ == 0)
            return 0;
    }
    return (char *)s;
}
