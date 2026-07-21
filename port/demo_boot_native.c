/* demo_boot_native.c -- boot the game natively: call its own startup_main.
 *
 * With the file/video/memory/timer/input shims + the DOS-int emulator (port/dosint.c) in
 * place, this hands control to the game's real entry (startup_main -> init -> main_game_loop).
 * Run from a directory with a lowercase data/ of the user's own files.
 */
#include <stdio.h>

extern void game_startup_main(int argc, char **argv);   /* game asm @0x24be8 */
extern void dosint_install(void);

int main(int argc, char **argv) {
    char *gargv[2];
    (void)argc; (void)argv;
    dosint_install();
    gargv[0] = "syndicate"; gargv[1] = NULL;
    fprintf(stderr, "[boot] calling game_startup_main...\n");
    game_startup_main(1, gargv);
    fprintf(stderr, "[boot] game_startup_main returned.\n");
    return 0;
}
