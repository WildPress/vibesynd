/* demo_video_native.c -- drive the game's own present path to an SDL window.
 *
 * Points g_screen_buf at the SDL framebuffer, loads a real full-screen image + palette
 * (the palette via the game's OWN loader), and calls the game's vga_planar_present -- which
 * is redirected to shim_vga_planar_present -> the SDL/GPU layer. So the game's data model and
 * present entry drive a real native window.
 */
#include "platform.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

extern unsigned char __dgroup[];
extern int  game_load_unpack_file(const char *path, void *buf);   /* game asm loader */
extern void game_vga_planar_present(void);                        /* redirected to the shim */
#define G_SCREEN_BUF (*(unsigned char **)(__dgroup + 0x5368))

int main(int argc, char **argv) {
    const char *dir = (argc > 1) ? argv[1] :
        "/mnt/c/Program Files (x86)/GOG Galaxy/Games/Syndicate Plus/SYNDICAT/DATA";
    char path[1024];
    static unsigned char pal[8192];
    FILE *f;

    plat_video_init(SYN_W, SYN_H);
    G_SCREEN_BUF = plat_framebuffer();          /* the game draws where SDL presents from */

    /* MCONSCR.DAT is a raw 320x200 frame -- load it into the game's screen buffer */
    snprintf(path, sizeof path, "%s/MCONSCR.DAT", dir);
    f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path); return 1; }
    fread(G_SCREEN_BUF, 1, SYN_W * SYN_H, f);
    fclose(f);

    /* palette via the GAME's own loader (open+read+RNC+close, all its asm) */
    snprintf(path, sizeof path, "%s/MSELECT.PAL", dir);
    if (game_load_unpack_file(path, pal) >= 0)
        plat_set_palette(pal);

    fprintf(stderr, "[demo] game_vga_planar_present -> SDL. ESC to quit.\n");
    while (!plat_want_quit()) {
        game_vga_planar_present();              /* the game's present entry, natively */
        plat_sleep_ms(16);
    }
    return 0;
}
