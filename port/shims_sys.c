/* shims_sys.c -- native replacements for the game's timer/input/BIOS-call routines.
 * The game is -4s (stack calling), so these are plain cdecl. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP_US(us) Sleep((us) >= 1000 ? (us) / 1000 : 1)   /* Win Sleep is ms-granular */
#else
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#define SLEEP_US(us) usleep(us)
#endif

extern unsigned char __dgroup[];
#define G_TIMER_TICK (*(volatile unsigned *)(__dgroup + 0x10b50))   /* loop waits on this */

/* --- timer: a background thread advances the tick the way the PIT ISR would.
 * The PIT runs at 1193182 Hz / divisor; the game programs the divisor, so honour it (a fixed
 * rate makes timed animations run fast/slow). Default 65536 = the 18.2 Hz power-on rate. --- */
static volatile int g_timer_run = 0;
static volatile unsigned g_tick_us = 54925;   /* 18.2 Hz until the game reprograms it */

static void timer_body(void) {
    while (g_timer_run) {
        SLEEP_US(g_tick_us);
        G_TIMER_TICK++;
    }
}

#ifdef _WIN32
static HANDLE g_timer_thread;
static DWORD WINAPI timer_loop(LPVOID arg) { (void)arg; timer_body(); return 0; }
void shim_install_timer_isr(void) {
    if (getenv("SYN_DEBUG")) fprintf(stderr, "[timer] install_timer_isr called\n");
    if (!g_timer_run) {
        g_timer_run = 1;
        g_timer_thread = CreateThread(NULL, 0, timer_loop, NULL, 0, NULL);
    }
}
void shim_uninstall_timer_isr(void) {
    if (g_timer_run) { g_timer_run = 0; WaitForSingleObject(g_timer_thread, INFINITE); }
}
#else
static pthread_t g_timer_thread;
static void *timer_loop(void *arg) {
    (void)arg;
    sigset_t m; sigemptyset(&m); sigaddset(&m, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &m, 0);   /* keep the profiler's SIGALRM off this thread */
    timer_body();
    return 0;
}
void shim_install_timer_isr(void) {
    if (getenv("SYN_DEBUG")) fprintf(stderr, "[timer] install_timer_isr called\n");
    if (!g_timer_run) {
        g_timer_run = 1;
        pthread_create(&g_timer_thread, 0, timer_loop, 0);
    }
}
void shim_uninstall_timer_isr(void) {
    if (g_timer_run) { g_timer_run = 0; pthread_join(g_timer_thread, 0); }
}
#endif

/* set the tick rate from a PIT divisor (called by the function shim and the port-IO emulator) */
void syn_set_pit(int divisor) {
    unsigned d = (unsigned)(divisor & 0xffff);
    if (d == 0) d = 65536;
    g_tick_us = (unsigned)(1000000.0 * d / 1193182.0 + 0.5);   /* PIT period in microseconds */
    if (getenv("SYN_DEBUG")) fprintf(stderr, "[timer] PIT divisor=%u -> %.1f Hz\n", d, 1193182.0 / d);
}
void shim_reprogram_pit_ch0(int divisor) { syn_set_pit(divisor); }
void shim_frame_throttle(void) { SLEEP_US(1000); }              /* yield a little */

/* --- input: keys come from the platform backend (shm ring, fed by the SDL viewer) --- */
extern int plat_poll_key(void);
int shim_poll_key(void) { return plat_poll_key(); }

/* --- int386(inter, inregs, outregs): the DOS/BIOS call helper. All uses are DOS/video that
 *     the other shims already handle, so this is a no-op returning 0. --- */
int shim_int386(int inter, void *inregs, void *outregs) {
    (void)inter; (void)inregs;
    if (outregs) memset(outregs, 0, 0x1c);   /* zero a REGS union so callers read 0 */
    return 0;
}
