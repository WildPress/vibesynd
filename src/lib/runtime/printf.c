/* C runtime: printf @ 0x3ad66 (CLIB3S, Watcom 9.5). Variadic formatter front-end
   for a stream/handle: set up a va_list at the first vararg and tail into the
   shared formatter core stream_reopen(dest, fmt, &args). The first arg 0xc09a is
   the output destination (a resolved data address, pushed as a literal -> exact
   bytes). Returns the core's result (character count). */
extern int stream_reopen(void *dest, const char *fmt, char **argp);

int printf(const char *fmt, ...)
{
    char *args = (char *)(&fmt + 1);

    return stream_reopen((void *)0xc09a, fmt, &args);
}
