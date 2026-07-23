/* demo_boot_native.c -- boot the game natively: call its own startup_main.
 *
 * With the file/video/memory/timer/input shims + the DOS-int emulator (port/dosint.c) in
 * place, this hands control to the game's real entry (startup_main -> init -> main_game_loop).
 * Run from a directory with a lowercase data/ of the user's own files.
 */
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#define SLEEP_US(us) Sleep((us) >= 1000 ? (us) / 1000 : 1)
#else
#include <pthread.h>
#include <unistd.h>
#define SLEEP_US(us) usleep(us)
#endif

extern void game_startup_main(int argc, char **argv);   /* game asm @0x24be8 */
extern void dosint_install(void);
extern void plat_video_init(int, int);
extern void plat_present_buf(const unsigned char *buf);
extern void plat_set_palette(const unsigned char *rgb768);
extern int  plat_want_quit(void);
extern void syn_shm_pump_input(void);                    /* shm keys -> game keyboard globals */
extern void dosint_get_dac(unsigned char *out768);       /* captured VGA DAC palette */

extern unsigned char __dgroup[];
#define G_SCREEN_BUF (*(unsigned char **)(__dgroup + 0x5368))

/* periodically snapshot the frame + the DAC palette so we can see the game draw. Present from
 * g_screen_buf (the game's offscreen buffer) if set, else the VGA window at 0xa0000. */
static void display_body(void) {
    for (;;) {
        unsigned char *src = G_SCREEN_BUF;
        SLEEP_US(33000);                       /* ~30 fps */
        syn_shm_pump_input();                  /* feed keys into the game's keyboard globals */
        /* palette is set by the upload_palette / flic_load_palette shims (portable C) now */
        plat_present_buf(src ? src : (const unsigned char *)0xa0000);
        if (plat_want_quit()) _exit(0);        /* viewer window closed */
    }
}
#ifdef _WIN32
static DWORD WINAPI display_loop(LPVOID arg) { (void)arg; display_body(); return 0; }
#else
static void *display_loop(void *arg) { (void)arg; display_body(); return 0; }
#endif

int main(int argc, char **argv) {
    char *gargv[2];
    (void)argc; (void)argv;
    dosint_install();
    plat_video_init(320, 200);
#ifdef _WIN32
    CreateThread(NULL, 0, display_loop, NULL, 0, NULL);
#else
    { pthread_t dt; pthread_create(&dt, 0, display_loop, 0); }
#endif
    gargv[0] = "syndicate"; gargv[1] = NULL;
    fprintf(stderr, "[boot] calling game_startup_main...\n");
    game_startup_main(1, gargv);
    fprintf(stderr, "[boot] game_startup_main returned.\n");
    return 0;
}
