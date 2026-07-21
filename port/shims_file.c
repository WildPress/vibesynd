/* shims_file.c -- native (POSIX/stdio) replacements for the game's DOS file-I/O leaves.
 *
 * The game is compiled -4s (486 STACK calling convention), so these leaves take their args
 * on the stack -- plain cdecl. tools/asm_emit_blob.py --shims redirects each DOS leaf's blob
 * entry to `shim_<name>` here. DOS file handles map straight onto POSIX fds.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* sopen(path, oflag, shflag[, pmode]) -- DOS/Watcom open. Low 2 bits of oflag select the
 * access mode (0 read, 1 write, 2 read/write), matching POSIX O_RDONLY/O_WRONLY/O_RDWR. */
int shim_sopen(const char *path, int oflag, int shflag) {
    int fd;
    (void)shflag;
    fd = open(path, (oflag & 3) | O_BINARY, 0666);
    return fd;   /* -1 on failure, as DOS expects */
}

int shim_read(int h, void *buf, unsigned n) {
    long r = (long)read(h, buf, n);
    return (int)r;
}

int shim_write(int h, const void *buf, unsigned n) {
    return (int)write(h, buf, n);
}

int shim_close(int h) {
    return close(h);
}

long shim_lseek(int h, long off, int whence) {
    return (long)lseek(h, off, whence);
}

long shim_tell(int h) {
    return (long)lseek(h, 0, SEEK_CUR);
}
