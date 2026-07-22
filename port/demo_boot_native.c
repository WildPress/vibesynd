/* demo_boot_native.c -- boot the game natively: call its own startup_main.
 *
 * With the file/video/memory/timer/input shims + the DOS-int emulator (port/dosint.c) in
 * place, this hands control to the game's real entry (startup_main -> init -> main_game_loop).
 * Run from a directory with a lowercase data/ of the user's own files.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

extern void game_startup_main(int argc, char **argv);   /* game asm @0x24be8 */
extern void dosint_install(void);
extern void plat_video_init(int, int);
extern void plat_present_buf(const unsigned char *buf);
extern void plat_set_palette(const unsigned char *rgb768);
extern int  plat_want_quit(void);
extern void dosint_get_dac(unsigned char *out768);       /* captured VGA DAC palette */

extern unsigned char __dgroup[];
#define G_SCREEN_BUF (*(unsigned char **)(__dgroup + 0x5368))

/* periodically snapshot the frame + the DAC palette so we can see the game draw. Present from
 * g_screen_buf (the game's offscreen buffer) if set, else the VGA window at 0xa0000. */
static void *display_loop(void *arg) {
    unsigned char pal[768];
    (void)arg;
    for (;;) {
        unsigned char *src = G_SCREEN_BUF;
        usleep(33000);                         /* ~30 fps */
        dosint_get_dac(pal);
        plat_set_palette(pal);
        plat_present_buf(src ? src : (const unsigned char *)0xa0000);
        if (plat_want_quit()) _exit(0);        /* viewer window closed */
    }
    return 0;
}

int main(int argc, char **argv) {
    char *gargv[2];
    pthread_t dt;
    (void)argc; (void)argv;
    dosint_install();
    plat_video_init(320, 200);
    pthread_create(&dt, 0, display_loop, 0);
    gargv[0] = "syndicate"; gargv[1] = NULL;
    fprintf(stderr, "[boot] calling game_startup_main...\n");
    game_startup_main(1, gargv);
    fprintf(stderr, "[boot] game_startup_main returned.\n");
    return 0;
}
