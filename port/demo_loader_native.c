/* demo_loader_native.c -- run the game's OWN file loader natively.
 *
 * Calls the game's game_load_unpack_file (open + read + RNC-decompress + close, all its own asm)
 * on a real compressed data file, with the DOS file-I/O leaves redirected to POSIX shims
 * (port/shims_file.c) and the DGROUP data model linked (port/dgroup.s). If the game's loader
 * pulls a file off disk and decompresses it correctly in a native process, the shim/redirect
 * path works end to end.
 *
 * The game is -4s (stack calling), so game_load_unpack_file(path, buf) is callable directly.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>

extern int game_load_unpack_file(const char *path, void *buf);   /* game asm @0x188e8 */
extern char __code[];                                       /* blob base = manifest 0xd748 */

static void on_segv(int sig, siginfo_t *si, void *uc) {
    unsigned eip = (unsigned)((ucontext_t *)uc)->uc_mcontext.gregs[REG_EIP];
    unsigned base = (unsigned)(unsigned long)__code;
    (void)sig;
    fprintf(stderr, "\nSEGV at eip=0x%x  fault_addr=%p\n", eip, si->si_addr);
    if (eip >= base)
        fprintf(stderr, "  -> manifest 0x%x (blob+0x%x)\n", 0xd748 + (eip - base), eip - base);
    _exit(2);
}

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] :
        "/mnt/c/Program Files (x86)/GOG Galaxy/Games/Syndicate Plus/SYNDICAT/DATA/HPALETTE.DAT";
    static unsigned char buf[4096];
    long i, mx = 0;
    int fd;

    static char altstk[64 * 1024];
    stack_t ss;
    struct sigaction sa;
    ss.ss_sp = altstk; ss.ss_size = sizeof altstk; ss.ss_flags = 0;
    sigaltstack(&ss, NULL);
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_segv;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGSEGV, &sa, NULL);

    memset(buf, 0, sizeof buf);
    fd = game_load_unpack_file(path, buf);       /* <-- the game's own loader, natively */
    fprintf(stderr, "game_load_unpack_file(\"%s\") returned %d\n", path, fd);

    for (i = 0; i < 768; i++) if (buf[i] > mx) mx = buf[i];
    printf("first 768 bytes: max value 0x%02lx\n", mx);
    if (mx > 0 && mx <= 0x3f) {
        printf("NATIVE-LOADER-OK: the game loaded + decompressed a 6-bit palette from disk\n");
        return 0;
    }
    printf("UNEXPECTED (expected a 6-bit palette, max <= 0x3f)\n");
    return 1;
}
