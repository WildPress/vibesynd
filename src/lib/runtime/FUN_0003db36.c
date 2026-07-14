/* C runtime: chktty @ 0x3db36 (CLIB3S). If the tty-checked flag (0x20) is unset and the fd is a
   tty (isatty), set 0x20; if no buffering-mode bits (0x7) are set, mark line-buffered (0x2). */
extern int FUN_0003c44d(int fd);
void FUN_0003db36(unsigned char *fp)
{
    if (!(fp[0xd] & 0x20)) {
        if (FUN_0003c44d(*(int *)(fp + 0x10)) != 0) {
            fp[0xd] |= 0x20;
            if (!(fp[0xd] & 7))
                fp[0xd] |= 2;
        }
    }
}
