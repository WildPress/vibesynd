/* shims_sys.c -- native replacements for the game's timer/input/BIOS-call routines.
 * The game is -4s (stack calling), so these are plain cdecl. */
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

extern unsigned char __dgroup[];
#define G_TIMER_TICK (*(volatile unsigned *)(__dgroup + 0x10b50))   /* loop waits on this */

/* --- timer: a background thread advances the tick the way the PIT ISR would --- */
static pthread_t g_timer_thread;
static volatile int g_timer_run = 0;

static void *timer_loop(void *arg) {
    (void)arg;
    sigset_t m; sigemptyset(&m); sigaddset(&m, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &m, 0);
    while (g_timer_run) {
        usleep(14000);          /* ~70 Hz, the game's reprogrammed PIT rate */
        G_TIMER_TICK++;
    }
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
void shim_reprogram_pit_ch0(int divisor) { (void)divisor; }   /* rate fixed at ~70 Hz above */
void shim_frame_throttle(void) { usleep(1000); }              /* yield a little */

/* --- input: no key / no BIOS keyboard for now --- */
int shim_poll_key(void) { return 0; }

/* --- int386(inter, inregs, outregs): the DOS/BIOS call helper. All uses are DOS/video that
 *     the other shims already handle, so this is a no-op returning 0. --- */
int shim_int386(int inter, void *inregs, void *outregs) {
    (void)inter; (void)inregs;
    if (outregs) memset(outregs, 0, 0x1c);   /* zero a REGS union so callers read 0 */
    return 0;
}
