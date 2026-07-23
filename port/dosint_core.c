/* dosint_core.c -- platform-independent DOS/BIOS int + port-I/O emulation.
 *
 * Lifted out of dosint.c so the same logic serves the POSIX signal handler and the Windows
 * Vectored Exception Handler. Operates only on the RG_* register file (dosint_core.h) plus
 * portable libc (open/read/write/lseek/close, fprintf, _exit) -- no ucontext, no Win32.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dosint_core.h"

#ifdef _WIN32
  #include <io.h>
  #include <fcntl.h>
  #define WRITE _write
  #define READ  _read
  #define OPEN  _open
  #define CLOSE _close
  #define LSEEK _lseek
  #ifndef O_BINARY
  #define O_BINARY 0x8000
  #endif
#else
  #include <unistd.h>
  #include <fcntl.h>
  #define WRITE write
  #define READ  read
  #define OPEN  open
  #define CLOSE close
  #define LSEEK lseek
  #ifndef O_BINARY
  #define O_BINARY 0
  #endif
#endif

extern char __code[];

#define AX(r) ((r)[RG_EAX] & 0xffff)
#define AH(r) (((r)[RG_EAX] >> 8) & 0xff)
#define AL(r) ((r)[RG_EAX] & 0xff)
#define SET_AL(r,v) ((r)[RG_EAX] = ((r)[RG_EAX] & ~0xffL) | ((v) & 0xff))
#define SET_AH(r,v) ((r)[RG_EAX] = ((r)[RG_EAX] & ~0xff00L) | (((v) & 0xff) << 8))
#define SET_AX(r,v) ((r)[RG_EAX] = ((r)[RG_EAX] & ~0xffffL) | ((v) & 0xffff))
#define SET_DL(r,v) ((r)[RG_EDX] = ((r)[RG_EDX] & ~0xffL) | ((v) & 0xff))
#define SET_DX(r,v) ((r)[RG_EDX] = ((r)[RG_EDX] & ~0xffffL) | ((v) & 0xffff))
#define SET_BX(r,v) ((r)[RG_EBX] = ((r)[RG_EBX] & ~0xffffL) | ((v) & 0xffff))
#define SET_CX(r,v) ((r)[RG_ECX] = ((r)[RG_ECX] & ~0xffffL) | ((v) & 0xffff))
#define CLC(r) ((r)[RG_EFL] &= ~1L)   /* clear carry: DOS "success" */
#define STC(r) ((r)[RG_EFL] |=  1L)

static char g_dta[128];    /* a Disk Transfer Area for AH=0x1a/0x2f */

/* captured VGA DAC palette (256 * r,g,b, 6-bit) */
unsigned char g_dac[768];
int g_dac_idx = 0;
void dosint_get_dac(unsigned char *out768) { int i; for (i = 0; i < 768; i++) out768[i] = g_dac[i]; }

/* mouse position + buttons, provided by the platform backend (weak default = no mouse). */
#ifdef _WIN32
void syn_get_mouse(int *x, int *y, int *b);   /* strong def supplied by the display backend */
void syn_set_pit(int divisor);
#pragma weak syn_get_mouse
#pragma weak syn_set_pit
void syn_get_mouse(int *x, int *y, int *b) { if (x) *x = 0; if (y) *y = 0; if (b) *b = 0; }
void syn_set_pit(int divisor) { (void)divisor; }
#else
void __attribute__((weak)) syn_get_mouse(int *x, int *y, int *b) {
    if (x) *x = 0; if (y) *y = 0; if (b) *b = 0;
}
void __attribute__((weak)) syn_set_pit(int divisor) { (void)divisor; }
#endif

/* INT 33h mouse driver. AX is the function number. Reports the platform mouse to the game. */
static int do_int33(long *r) {
    int fn = (int)(r[RG_EAX] & 0xffff), mx = 0, my = 0, mb = 0;
    syn_get_mouse(&mx, &my, &mb);
    switch (fn) {
    case 0x00: SET_AX(r, 0xffff); SET_BX(r, 2); return 1;   /* reset: installed, 2 buttons */
    case 0x03: {                                            /* get position + button state */
        /* SDL mask (left=1,middle=2,right=4) -> INT33 (bit0 left, bit1 right, bit2 middle) */
        int b33 = (mb & 1) | ((mb & 4) ? 2 : 0) | ((mb & 2) ? 4 : 0);
        SET_BX(r, b33); SET_CX(r, mx * 2); SET_DX(r, my * 2); return 1;  /* game uses 638x398, /2 */
    }
    case 0x01: case 0x02: return 1;                         /* show / hide cursor */
    case 0x04: return 1;                                    /* set position */
    default: SET_AX(r, 0); return 1;
    }
}

static int do_int21(int ah, long *r) {
    switch (ah) {
    case 0x3f: {   /* read  */
        long n = READ((int)(r[RG_EBX] & 0xffff), (void *)(long)r[RG_EDX], (unsigned)r[RG_ECX]);
        r[RG_EAX] = (n < 0) ? 0 : (unsigned)n; if (n < 0) STC(r); else CLC(r); return 1;
    }
    case 0x40: {   /* write */
        long n = WRITE((int)(r[RG_EBX] & 0xffff), (void *)(long)r[RG_EDX], (unsigned)r[RG_ECX]);
        r[RG_EAX] = (n < 0) ? 0 : (unsigned)n; if (n < 0) STC(r); else CLC(r); return 1;
    }
    case 0x42: {   /* lseek: CX:DX offset, AL whence -> DX:AX */
        long off = (long)((r[RG_ECX] & 0xffff) << 16 | (r[RG_EDX] & 0xffff));
        long pos = LSEEK((int)(r[RG_EBX] & 0xffff), off, (int)(r[RG_EAX] & 0xff));
        if (pos < 0) { STC(r); return 1; }
        r[RG_EAX] = (unsigned)pos & 0xffff; SET_DX(r, (pos >> 16) & 0xffff); CLC(r); return 1;
    }
    case 0x3d: case 0x3c: {   /* open / creat: EDX = flat path */
        int fl = (ah == 0x3c) ? (O_WRONLY | O_CREAT | O_TRUNC) : ((r[RG_EAX] & 3) | O_CREAT);
        int fd = OPEN((const char *)(long)r[RG_EDX], fl | O_BINARY, 0666);
        r[RG_EAX] = (fd < 0) ? 2 : (unsigned)fd; if (fd < 0) STC(r); else CLC(r); return 1;
    }
    case 0x3e:     /* close */
        CLOSE((int)(r[RG_EBX] & 0xffff)); CLC(r); return 1;
    case 0x30: SET_AX(r, 0x0005); SET_BX(r, 0); SET_CX(r, 0); CLC(r); return 1; /* DOS 5.0 */
    case 0x19: SET_AL(r, 2);       CLC(r); return 1;   /* current drive = C: */
    case 0x2c: SET_CX(r, 0x0c00); SET_DX(r, 0); CLC(r); return 1;  /* time 12:00:00 */
    case 0x2a: SET_CX(r, 1995); SET_DX(r, 0x0203); CLC(r); return 1; /* date 1995-02-03 */
    case 0x37: SET_DL(r, '/'); SET_AL(r, 0); CLC(r); return 1;      /* switch char */
    case 0x25: CLC(r); return 1;   /* set interrupt vector: ignore */
    case 0x35: SET_BX(r, 0); CLC(r); return 1;   /* get vector -> null */
    case 0x1a: memcpy(g_dta, (void *)(long)r[RG_EDX], 0); CLC(r); return 1; /* set DTA: note ptr */
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
    if (WRITE(2, b, n) < 0) { /* ignore */ }
}

void syn_dos_trap(long *r, unsigned fault_addr, int have_fault_addr, const char *signame) {
    unsigned eip = (unsigned)r[RG_EIP];
    unsigned char *p = (unsigned char *)(long)eip;
    unsigned base = (unsigned)(unsigned long)__code;

    /* if EIP itself is unmapped (a bad jump), reading p[] below would nest-fault -> silent crash.
     * Detect via the instruction-fetch fault (fault address == EIP) and report the caller
     * instead, without touching p. */
    if (have_fault_addr && fault_addr == eip) {
        unsigned *sp = (unsigned *)(long)r[RG_ESP];
        unsigned ret = sp ? sp[0] : 0;
        wr_hex("BAD-JUMP eip=", eip);
        wr_hex("  caller-ret=", ret);
        if (ret >= base && ret < base + 0x40000) wr_hex("  caller-manifest=", 0xd748 + (ret - base));
        _exit(4);
    }

    if (getenv("SYN_TRAPLOG") && g_traps++ < 60 && eip >= base)
        fprintf(stderr, "[trap %lu] manifest 0x%x op=%02x %02x %02x\n",
                g_traps, 0xd748 + (eip - base), p ? p[0] : 0, p ? p[1] : 0, p ? p[2] : 0);

    /* SYN_DOSLOG: log each UNIQUE trap site once -- the exact DOS-instruction surface to port. */
    if (getenv("SYN_DOSLOG") && eip >= base && eip < base + 0x40000) {
        static unsigned seen[512]; static int nseen = 0; int i, found = 0;
        for (i = 0; i < nseen; i++) if (seen[i] == eip) { found = 1; break; }
        if (!found && nseen < 512) {
            seen[nseen++] = eip;
            fprintf(stderr, "[dos] manifest 0x%x op=%02x %02x %02x\n",
                    0xd748 + (eip - base), p ? p[0] : 0, p ? p[1] : 0, p ? p[2] : 0);
        }
    }

    if (p && p[0] == 0xCD) {              /* int nn */
        int n = p[1], ah = AH(r), handled = 0;
        if (n == 0x21 && ah == 0x4c) {    /* DOS terminate: log where from before exiting */
            unsigned *sp = (unsigned *)(long)r[RG_ESP];
            wr_hex("DOS-EXIT eip=", eip);
            if (eip >= base && eip < base + 0x40000) wr_hex("  manifest=", 0xd748 + (eip - base));
            wr_hex("  ret=", sp ? sp[0] : 0);
        }
        if (n == 0x21) handled = do_int21(ah, r);
        else if (n == 0x10) { CLC(r); handled = 1; }   /* video BIOS: no-op */
        else if (n == 0x16) {                           /* keyboard BIOS */
            if (ah == 0x01) { r[RG_EFL] |= 0x40; }      /* ZF: no key */
            SET_AX(r, 0); handled = 1;
        } else if (n == 0x33) { handled = do_int33(r); }   /* mouse -> the platform mouse */
        else if (n == 0x31) { CLC(r); handled = 1; }          /* DPMI: succeed */
        if (handled) { r[RG_EIP] = eip + 2; return; }
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
        if (q == 0xFA || q == 0xFB) { r[RG_EIP] = eip + i0 + 1; return; }     /* cli / sti */
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
            r[RG_EIP] = eip + len; return;
        }
        if (q == 0x07 || q == 0x17 || q == 0x1F) { r[RG_EIP] = eip + i0 + 1; return; } /* pop es/ss/ds */
        if (q == 0x0F && (p[i0+1] == 0xA1 || p[i0+1] == 0xA9)) { r[RG_EIP] = eip + i0 + 2; return; } /* pop fs/gs */
    }

    /* rep outsb (F3 6E): the DAC palette blast `rep outs dx,[esi]` to port 0x3c9 -- capture it */
    if (p && p[0] == 0xF3 && p[1] == 0x6E) {
        unsigned port = r[RG_EDX] & 0xffff, cnt = r[RG_ECX];
        unsigned char *src = (unsigned char *)(long)r[RG_ESI];
        if (port == 0x3c9) { unsigned i; for (i = 0; i < cnt; i++) g_dac[(g_dac_idx++) % 768] = src[i]; }
        r[RG_ESI] += cnt; r[RG_ECX] = 0; r[RG_EIP] = eip + 2; return;
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
                unsigned port = imm ? p[idx + 1] : (r[RG_EDX] & 0xffff);
                unsigned val = r[RG_EAX] & 0xff;
                if (port == 0x3c8) g_dac_idx = val * 3;
                else if (port == 0x3c9) g_dac[(g_dac_idx++) % 768] = val;
                else if (port == 0x40) {                 /* PIT channel 0 divisor: low byte then high */
                    static int lo = -1;
                    if (lo < 0) lo = val;
                    else { syn_set_pit(lo | (val << 8)); lo = -1; }
                }
                r[RG_EIP] = eip + len; return;
            }
            if (is_in) {
                static int t = 0; t ^= 0xff;
                if (w16) SET_AX(r, t); else SET_AL(r, t);
                r[RG_EIP] = eip + len; return;
            }
        }
    }
    /* crash-safe report: registers only, no dereferencing (EBP is often data here) */
    fprintf(stderr, "\n%s eip=0x%x fault=0x%x esp=0x%x ebp=0x%x", signame,
            eip, have_fault_addr ? fault_addr : 0, (unsigned)r[RG_ESP], (unsigned)r[RG_EBP]);
    if (eip >= base && eip < base + 0x40000)
        fprintf(stderr, "  -> manifest 0x%x (blob+0x%x)", 0xd748 + (eip - base), eip - base);
    else
        fprintf(stderr, "  (eip non-blob)");
    fprintf(stderr, "  bytes %02x %02x %02x\n", p ? p[0] : 0, p ? p[1] : 0, p ? p[2] : 0);
    fflush(stderr);
    _exit(2);
}
