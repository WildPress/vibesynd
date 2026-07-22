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
#include <fcntl.h>
#ifdef O_BINARY
#define O_BINARY_ O_BINARY
#else
#define O_BINARY_ 0
#endif

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

/* captured VGA DAC palette (256 * r,g,b, 6-bit) */
unsigned char g_dac[768];
int g_dac_idx = 0;
void dosint_get_dac(unsigned char *out768) { int i; for (i = 0; i < 768; i++) out768[i] = g_dac[i]; }

/* DOS/4GW extends int21 file calls to 32-bit: EBX=handle, ECX=count, EDX=flat buf/path. */
/* mouse position + buttons, provided by the platform backend (weak default = no mouse). */
void __attribute__((weak)) syn_get_mouse(int *x, int *y, int *b) {
    if (x) *x = 0; if (y) *y = 0; if (b) *b = 0;
}
/* the game programming the PIT divisor (weak default = ignore; shims_sys sets the tick rate). */
void __attribute__((weak)) syn_set_pit(int divisor) { (void)divisor; }

/* INT 33h mouse driver. AX is the function number. Reports the shm mouse to the game. */
static int do_int33(greg_t *r) {
    int fn = (int)(r[REG_EAX] & 0xffff), mx = 0, my = 0, mb = 0;
    syn_get_mouse(&mx, &my, &mb);
    switch (fn) {
    case 0x00: SET_AX(r, 0xffff); SET_BX(r, 2); return 1;   /* reset: installed, 2 buttons */
    case 0x03: {                                            /* get position + button state */
        /* SDL mask (left=1,middle=2,right=4) -> INT33 (bit0 left, bit1 right, bit2 middle) */
        int b33 = (mb & 1) | ((mb & 4) ? 2 : 0) | ((mb & 2) ? 4 : 0);
        SET_BX(r, b33); SET_CX(r, mx * 2); SET_DX(r, my); return 1;   /* mode-13h x is 0..639 */
    }
    case 0x01: case 0x02: return 1;                         /* show / hide cursor */
    case 0x04: return 1;                                    /* set position */
    default: SET_AX(r, 0); return 1;
    }
}

static int do_int21(int ah, greg_t *r) {
    switch (ah) {
    case 0x3f: {   /* read  */
        long n = read((int)(r[REG_EBX] & 0xffff), (void *)(long)r[REG_EDX], (size_t)r[REG_ECX]);
        r[REG_EAX] = (n < 0) ? 0 : (unsigned)n; if (n < 0) STC(r); else CLC(r); return 1;
    }
    case 0x40: {   /* write */
        long n = write((int)(r[REG_EBX] & 0xffff), (void *)(long)r[REG_EDX], (size_t)r[REG_ECX]);
        r[REG_EAX] = (n < 0) ? 0 : (unsigned)n; if (n < 0) STC(r); else CLC(r); return 1;
    }
    case 0x42: {   /* lseek: CX:DX offset, AL whence -> DX:AX */
        long off = (long)((r[REG_ECX] & 0xffff) << 16 | (r[REG_EDX] & 0xffff));
        long pos = lseek((int)(r[REG_EBX] & 0xffff), off, (int)(r[REG_EAX] & 0xff));
        if (pos < 0) { STC(r); return 1; }
        r[REG_EAX] = (unsigned)pos & 0xffff; SET_DX(r, (pos >> 16) & 0xffff); CLC(r); return 1;
    }
    case 0x3d: case 0x3c: {   /* open / creat: EDX = flat path */
        int fl = (ah == 0x3c) ? (O_WRONLY | O_CREAT | O_TRUNC) : ((r[REG_EAX] & 3) | O_CREAT);
        int fd = open((const char *)(long)r[REG_EDX], fl | O_BINARY_, 0666);
        r[REG_EAX] = (fd < 0) ? 2 : (unsigned)fd; if (fd < 0) STC(r); else CLC(r); return 1;
    }
    case 0x3e:     /* close */
        close((int)(r[REG_EBX] & 0xffff)); CLC(r); return 1;
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

static unsigned long g_traps = 0;

/* async-signal-safe: "label 0xVALUE\n" */
static void wr_hex(const char *s, unsigned v) {
    char b[64]; int i, n = 0;
    while (*s) b[n++] = *s++;
    b[n++] = '0'; b[n++] = 'x';
    for (i = 28; i >= 0; i -= 4) { int d = (v >> i) & 0xf; b[n++] = d < 10 ? '0' + d : 'a' + d - 10; }
    b[n++] = '\n';
    if (write(2, b, n) < 0) { /* ignore */ }
}

static void on_trap(int sig, siginfo_t *si, void *ucv) {
    ucontext_t *uc = ucv;
    greg_t *r = uc->uc_mcontext.gregs;
    unsigned eip = (unsigned)r[REG_EIP];
    unsigned char *p = (unsigned char *)(long)eip;
    unsigned base = (unsigned)(unsigned long)__code;

    /* if EIP itself is unmapped (a bad jump), reading p[] below would nest-fault -> core dump
     * with no message. Detect via the instruction-fetch fault (fault address == EIP) and report
     * the caller instead, without touching p. */
    if (sig == SIGSEGV && (unsigned)(long)si->si_addr == eip) {
        unsigned *sp = (unsigned *)(long)r[REG_ESP];
        unsigned ret = sp ? sp[0] : 0;
        wr_hex("BAD-JUMP eip=", eip);
        wr_hex("  caller-ret=", ret);
        if (ret >= base && ret < base + 0x40000) wr_hex("  caller-manifest=", 0xd748 + (ret - base));
        _exit(4);
    }

    if (getenv("SYN_TRAPLOG") && g_traps++ < 60 && eip >= base)
        fprintf(stderr, "[trap %lu] manifest 0x%x op=%02x %02x %02x\n",
                g_traps, 0xd748 + (eip - base), p ? p[0] : 0, p ? p[1] : 0, p ? p[2] : 0);

    if (p && p[0] == 0xCD) {              /* int nn */
        int n = p[1], ah = AH(r), handled = 0;
        if (n == 0x21 && ah == 0x4c) {    /* DOS terminate: log where from before exiting */
            unsigned *sp = (unsigned *)(long)r[REG_ESP];
            wr_hex("DOS-EXIT eip=", eip);
            if (eip >= base && eip < base + 0x40000) wr_hex("  manifest=", 0xd748 + (eip - base));
            wr_hex("  ret=", sp ? sp[0] : 0);
        }
        if (n == 0x21) handled = do_int21(ah, r);
        else if (n == 0x10) { CLC(r); handled = 1; }   /* video BIOS: no-op */
        else if (n == 0x16) {                           /* keyboard BIOS */
            if (ah == 0x01) { uc->uc_mcontext.gregs[REG_EFL] |= 0x40; }  /* ZF: no key */
            SET_AX(r, 0); handled = 1;
        } else if (n == 0x33) { handled = do_int33(r); }   /* mouse -> the shm mouse */
        else if (n == 0x31) { CLC(r); handled = 1; }          /* DPMI: succeed */
        if (handled) { r[REG_EIP] = eip + 2; return; }
        fprintf(stderr, "\nunhandled int 0x%02x AH=0x%02x at manifest 0x%x\n",
                n, ah, base ? 0xd748 + (eip - base) : eip);
        _exit(3);
    }

    /* privileged instructions that are meaningless in a flat userspace process: skip them.
     * cli/sti (interrupt flag), and segment-register loads `mov Sreg,r/m` / `pop Sreg`
     * (es/ds/fs/gs stay the flat selector, which is what the code wants anyway). */
    if (p) {
        int i0 = (p[0] == 0x66 || p[0] == 0x67) ? 1 : 0;
        unsigned char q = p[i0];
        if (q == 0xFA || q == 0xFB) { r[REG_EIP] = eip + i0 + 1; return; }     /* cli / sti */
        if (q == 0x8E) {   /* mov Sreg, r/m16 -- skip whole ModRM instruction */
            unsigned char modrm = p[i0 + 1];
            int len = i0 + 2;
            int mod = modrm >> 6, rm = modrm & 7;
            if (mod != 3) {
                if (rm == 4) len++;                       /* SIB */
                if (mod == 1) len += 1;
                else if (mod == 2) len += 4;
                else if (mod == 0 && rm == 5) len += 4;   /* disp32 */
            }
            r[REG_EIP] = eip + len; return;
        }
        if (q == 0x07 || q == 0x17 || q == 0x1F) { r[REG_EIP] = eip + i0 + 1; return; } /* pop es/ss/ds */
        if (q == 0x0F && (p[i0+1] == 0xA1 || p[i0+1] == 0xA9)) { r[REG_EIP] = eip + i0 + 2; return; } /* pop fs/gs */
    }

    /* rep outsb (F3 6E): the DAC palette blast `rep outs dx,[esi]` to port 0x3c9 -- capture it */
    if (p && p[0] == 0xF3 && p[1] == 0x6E) {
        unsigned port = r[REG_EDX] & 0xffff, cnt = r[REG_ECX];
        unsigned char *src = (unsigned char *)(long)r[REG_ESI];
        if (port == 0x3c9) { unsigned i; for (i = 0; i < cnt; i++) g_dac[(g_dac_idx++) % 768] = src[i]; }
        r[REG_ESI] += cnt; r[REG_ECX] = 0; r[REG_EIP] = eip + 2; return;
    }
    /* emulate port I/O (in/out) -- VGA/PIT/DAC registers. `in` returns a toggling value so
     * retrace-wait loops (wait for a bit set, then clear) terminate; `out` is a no-op except
     * the VGA DAC (0x3c8 index / 0x3c9 data), which we capture as the palette. */
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
            if (is_out) {
                unsigned port = imm ? p[idx + 1] : (r[REG_EDX] & 0xffff);
                unsigned val = r[REG_EAX] & 0xff;
                if (port == 0x3c8) g_dac_idx = val * 3;
                else if (port == 0x3c9) g_dac[(g_dac_idx++) % 768] = val;
                else if (port == 0x40) {                 /* PIT channel 0 divisor: low byte then high */
                    static int lo = -1;
                    if (lo < 0) lo = val;
                    else { syn_set_pit(lo | (val << 8)); lo = -1; }
                }
                r[REG_EIP] = eip + len; return;
            }
            if (is_in) {
                static int t = 0; t ^= 0xff;
                if (w16) SET_AX(r, t); else SET_AL(r, t);
                r[REG_EIP] = eip + len; return;
            }
        }
    }
    /* crash-safe report: registers only, no dereferencing (EBP is often data here) */
    fprintf(stderr, "\n%s eip=0x%x fault=%p esp=0x%x ebp=0x%x", strsignal(sig),
            eip, si->si_addr, (unsigned)r[REG_ESP], (unsigned)r[REG_EBP]);
    if (eip >= base && eip < base + 0x40000)
        fprintf(stderr, "  -> manifest 0x%x (blob+0x%x)", 0xd748 + (eip - base), eip - base);
    else
        fprintf(stderr, "  (eip non-blob)");
    fprintf(stderr, "  bytes %02x %02x %02x\n", p ? p[0] : 0, p ? p[1] : 0, p ? p[2] : 0);
    fflush(stderr);
    _exit(2);
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
