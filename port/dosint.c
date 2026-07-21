/* dosint.c -- emulate the DOS/BIOS `int` calls the game's CLIB stragglers still make.
 *
 * Most hardware routines are shimmed at the function level, but a handful of CLIB startup
 * helpers (switch char, DOS version, get-time, ioctl, ...) issue a raw `int 0x21` / `int 0x10`
 * / `int 0x16`. In a native 32-bit process those trap. This installs a SIGSEGV/SIGILL handler
 * that, when the faulting instruction is `CD nn` (int nn), services the call from the trapped
 * register context and steps EIP past it -- a tiny DOS shim covering the startup path.
 *
 * Anything it can't service prints the fault (mapped to a manifest function) and exits, so
 * bring-up proceeds one real obstacle at a time.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

extern char __code[];

#define AX(r) ((r)[REG_EAX] & 0xffff)
#define AH(r) (((r)[REG_EAX] >> 8) & 0xff)
#define AL(r) ((r)[REG_EAX] & 0xff)
#define SET_AL(r,v) ((r)[REG_EAX] = ((r)[REG_EAX] & ~0xffL) | ((v) & 0xff))
#define SET_AH(r,v) ((r)[REG_EAX] = ((r)[REG_EAX] & ~0xff00L) | (((v) & 0xff) << 8))
#define SET_AX(r,v) ((r)[REG_EAX] = ((r)[REG_EAX] & ~0xffffL) | ((v) & 0xffff))
#define SET_DL(r,v) ((r)[REG_EDX] = ((r)[REG_EDX] & ~0xffL) | ((v) & 0xff))
#define SET_DX(r,v) ((r)[REG_EDX] = ((r)[REG_EDX] & ~0xffffL) | ((v) & 0xffff))
#define SET_BX(r,v) ((r)[REG_EBX] = ((r)[REG_EBX] & ~0xffffL) | ((v) & 0xffff))
#define SET_CX(r,v) ((r)[REG_ECX] = ((r)[REG_ECX] & ~0xffffL) | ((v) & 0xffff))
#define CLC(r) ((r)[REG_EFL] &= ~1L)   /* clear carry: DOS "success" */
#define STC(r) ((r)[REG_EFL] |=  1L)

static char g_dta[128];    /* a Disk Transfer Area for AH=0x1a/0x2f */

static int do_int21(int ah, greg_t *r) {
    switch (ah) {
    case 0x30: SET_AX(r, 0x0005); SET_BX(r, 0); SET_CX(r, 0); CLC(r); return 1; /* DOS 5.0 */
    case 0x19: SET_AL(r, 2);       CLC(r); return 1;   /* current drive = C: */
    case 0x2c: SET_CX(r, 0x0c00); SET_DX(r, 0); CLC(r); return 1;  /* time 12:00:00 */
    case 0x2a: SET_CX(r, 1995); SET_DX(r, 0x0203); CLC(r); return 1; /* date 1995-02-03 */
    case 0x37: SET_DL(r, '/'); SET_AL(r, 0); CLC(r); return 1;      /* switch char */
    case 0x25: CLC(r); return 1;   /* set interrupt vector: ignore */
    case 0x35: SET_BX(r, 0); CLC(r); return 1;   /* get vector -> null */
    case 0x1a: memcpy(g_dta, (void *)(long)r[REG_EDX], 0); CLC(r); return 1; /* set DTA: note ptr */
    case 0x44: SET_DX(r, 0); CLC(r); return 1;   /* ioctl: handle is a file, not a device */
    case 0x4a: CLC(r); return 1;   /* resize memory block: succeed */
    case 0x48: SET_AX(r, 0x2000); CLC(r); return 1; /* alloc paras -> fake segment */
    case 0x49: CLC(r); return 1;   /* free block */
    case 0x4c: _exit(AL(r));       /* terminate */
    default:   CLC(r); return 1;   /* unknown: pretend success and continue */
    }
}

static void on_trap(int sig, siginfo_t *si, void *ucv) {
    ucontext_t *uc = ucv;
    greg_t *r = uc->uc_mcontext.gregs;
    unsigned eip = (unsigned)r[REG_EIP];
    unsigned char *p = (unsigned char *)(long)eip;
    unsigned base = (unsigned)(unsigned long)__code;

    if (p && p[0] == 0xCD) {              /* int nn */
        int n = p[1], ah = AH(r), handled = 0;
        if (n == 0x21) handled = do_int21(ah, r);
        else if (n == 0x10) { CLC(r); handled = 1; }   /* video BIOS: no-op */
        else if (n == 0x16) {                           /* keyboard BIOS */
            if (ah == 0x01) { uc->uc_mcontext.gregs[REG_EFL] |= 0x40; }  /* ZF: no key */
            SET_AX(r, 0); handled = 1;
        } else if (n == 0x33) { SET_AX(r, 0); handled = 1; }  /* mouse: absent */
        else if (n == 0x31) { CLC(r); handled = 1; }          /* DPMI: succeed */
        if (handled) { r[REG_EIP] = eip + 2; return; }
        fprintf(stderr, "\nunhandled int 0x%02x AH=0x%02x at manifest 0x%x\n",
                n, ah, base ? 0xd748 + (eip - base) : eip);
        _exit(3);
    }

    /* emulate port I/O (in/out) -- VGA/PIT registers. `in` returns a toggling value so
     * retrace-wait loops (wait for a bit set, then clear) terminate; `out` is a no-op. */
    if (p) {
        int idx = 0, w16 = 0;
        unsigned char op;
        if (p[idx] == 0x66) { w16 = 1; idx++; }
        op = p[idx];
        {
            int is_in = (op == 0xEC || op == 0xED || op == 0xE4 || op == 0xE5);
            int is_out = (op == 0xEE || op == 0xEF || op == 0xE6 || op == 0xE7);
            int imm = (op >= 0xE4 && op <= 0xE7);
            int len = idx + 1 + (imm ? 1 : 0);
            if (is_in) {
                static int t = 0; t ^= 0xff;
                if (w16) SET_AX(r, t); else SET_AL(r, t);
                r[REG_EIP] = eip + len; return;
            }
            if (is_out) { r[REG_EIP] = eip + len; return; }
        }
    }
    fprintf(stderr, "\n%s eip=0x%x fault=%p", strsignal(sig), eip, si->si_addr);
    if (eip >= base) fprintf(stderr, "  -> manifest 0x%x", 0xd748 + (eip - base));
    fprintf(stderr, "\n");
    /* walk the ebp chain for a manifest backtrace */
    {
        unsigned *fp = (unsigned *)(long)r[REG_EBP];
        int depth;
        for (depth = 0; depth < 12 && fp; depth++) {
            unsigned ret = fp[1];
            if (ret >= base && ret < base + 0x40000)
                fprintf(stderr, "  [%d] manifest 0x%x\n", depth, 0xd748 + (ret - base));
            else { fprintf(stderr, "  [%d] 0x%x (non-blob)\n", depth, ret); break; }
            fp = (unsigned *)(long)fp[0];
        }
    }
    _exit(2);
}

void dosint_install(void) {
    static char altstk[64 * 1024];
    stack_t ss;
    struct sigaction sa;
    ss.ss_sp = altstk; ss.ss_size = sizeof altstk; ss.ss_flags = 0;
    sigaltstack(&ss, NULL);
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_trap;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
}
