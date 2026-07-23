/* dosint.c -- POSIX adapter for the DOS/BIOS int emulator (core: dosint_core.c).
 *
 * A handful of CLIB startup helpers issue a raw `int 0x21` / `int 0x10` / `int 0x16`, plus the
 * game does direct VGA/PIT port I/O. In a native 32-bit process those trap. This installs a
 * SIGSEGV/SIGILL handler that marshals the trapped ucontext register file into the portable
 * RG_* array, hands it to syn_dos_trap(), and writes the (possibly advanced) registers back so
 * the faulting thread resumes. The Windows equivalent is dosint_win.c (a Vectored Exception
 * Handler); both share dosint_core.c.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include "dosint_core.h"

extern char __code[];

/* marshal the platform gregs <-> our portable RG_* file */
static void gregs_to_rg(greg_t *g, long *r) {
    r[RG_EAX] = g[REG_EAX]; r[RG_EBX] = g[REG_EBX]; r[RG_ECX] = g[REG_ECX]; r[RG_EDX] = g[REG_EDX];
    r[RG_ESI] = g[REG_ESI]; r[RG_EDI] = g[REG_EDI]; r[RG_EBP] = g[REG_EBP]; r[RG_ESP] = g[REG_ESP];
    r[RG_EIP] = g[REG_EIP]; r[RG_EFL] = g[REG_EFL];
}
static void rg_to_gregs(long *r, greg_t *g) {
    g[REG_EAX] = r[RG_EAX]; g[REG_EBX] = r[RG_EBX]; g[REG_ECX] = r[RG_ECX]; g[REG_EDX] = r[RG_EDX];
    g[REG_ESI] = r[RG_ESI]; g[REG_EDI] = r[RG_EDI]; g[REG_EBP] = r[RG_EBP]; g[REG_ESP] = r[RG_ESP];
    g[REG_EIP] = r[RG_EIP]; g[REG_EFL] = r[RG_EFL];
}

static void on_trap(int sig, siginfo_t *si, void *ucv) {
    ucontext_t *uc = ucv;
    greg_t *g = uc->uc_mcontext.gregs;
    long r[RG_N];
    gregs_to_rg(g, r);
    syn_dos_trap(r, (unsigned)(long)si->si_addr, sig == SIGSEGV, strsignal(sig));
    rg_to_gregs(r, g);
}

/* --- sampling profiler: prints where execution is, to find spin loops --- */
#include <sys/time.h>
static int g_samples = 0;
static void on_alrm(int sig, siginfo_t *si, void *ucv) {
    ucontext_t *uc = ucv;
    unsigned eip = (unsigned)uc->uc_mcontext.gregs[REG_EIP];
    unsigned base = (unsigned)(unsigned long)__code;
    (void)sig; (void)si;
    if (g_samples++ < 12) {
        if (eip >= base && eip < base + 0x40000)
            fprintf(stderr, "[prof %d] manifest 0x%x\n", g_samples, 0xd748 + (eip - base));
        else
            fprintf(stderr, "[prof %d] 0x%x (non-blob)\n", g_samples, eip);
    }
}

#include <sys/mman.h>
void dosint_install(void) {
    static char altstk[64 * 1024];
    stack_t ss;
    struct sigaction sa;

    /* back the VGA memory window (0xa0000..0xc0000) so the game's direct-to-VGA writes
     * (flic_play, mode-13h linear copies) land in real memory instead of faulting. */
    mmap((void *)0xa0000, 0x20000, PROT_READ | PROT_WRITE | PROT_EXEC,
         MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ss.ss_sp = altstk; ss.ss_size = sizeof altstk; ss.ss_flags = 0;
    sigaltstack(&ss, NULL);
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_trap;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);

    if (getenv("SYN_PROF")) {
        struct sigaction pa;
        struct itimerval it;
        memset(&pa, 0, sizeof pa);
        pa.sa_sigaction = on_alrm; pa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESTART;
        sigaction(SIGALRM, &pa, NULL);
        it.it_interval.tv_sec = 0; it.it_interval.tv_usec = 500000;
        it.it_value = it.it_interval;
        int rc = setitimer(ITIMER_REAL, &it, NULL);
        fprintf(stderr, "[dosint] profiler armed rc=%d\n", rc);
    }
}
