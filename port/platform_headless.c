/* platform_headless.c -- a no-window platform backend (implements platform.h).
 *
 * plat_present converts the 8-bit framebuffer through the palette and writes it as a PPM,
 * so the video shims + the game's present path can be verified pixel-for-pixel without a
 * GPU/SDL context (and without a 32-bit SDL build). Presents once, then asks to quit.
 */
#include "platform.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static uint8_t  g_fb[SYN_W * SYN_H];
static uint8_t  g_pal[768];
static uint32_t g_rgba[SYN_W * SYN_H];
static int      g_frames = 0;
static const char *g_out = "port/gen/blob/frame.ppm";

void plat_video_init(int w, int h) {
    (void)w; (void)h;
    const char *e = getenv("SYN_FRAME_OUT");
    if (e) g_out = e;
}
uint8_t *plat_framebuffer(void) { return g_fb; }
void plat_set_palette(const uint8_t *rgb768) { memcpy(g_pal, rgb768, 768); }

void plat_present(void) {
    FILE *f = fopen(g_out, "wb");
    int i;
    gfx_indexed_to_rgba(g_fb, SYN_W, SYN_H, g_pal, g_rgba);
    if (f) {
        fprintf(f, "P6\n%d %d\n255\n", SYN_W, SYN_H);
        for (i = 0; i < SYN_W * SYN_H; i++) {
            uint32_t p = g_rgba[i];
            unsigned char rgb[3] = { p & 0xff, (p >> 8) & 0xff, (p >> 16) & 0xff };
            fwrite(rgb, 1, 3, f);
        }
        fclose(f);
        fprintf(stderr, "[headless] wrote %s\n", g_out);
    }
    g_frames++;
}

/* present an arbitrary 8-bit 320x200 buffer (e.g. the VGA window at 0xa0000) via the palette */
void plat_present_buf(const unsigned char *buf) {
    memcpy(g_fb, buf, SYN_W * SYN_H);
    plat_present();
}

void syn_shm_pump_input(void) {}   /* headless: no input source to pump */
int  plat_poll_key(void) { return 0; }
void plat_mouse(int *x, int *y, int *b) { if (x) *x = 0; if (y) *y = 0; if (b) *b = 0; }
int  plat_want_quit(void) { return g_frames >= 1; }
uint32_t plat_ticks_ms(void) { return 0; }
void plat_sleep_ms(uint32_t ms) { (void)ms; }
void plat_audio_init(void) {}
void plat_play_sound(int id) { (void)id; }
void plat_play_music(int t) { (void)t; }
void *plat_fopen(const char *p, const char *m) { return fopen(p, m); }
long plat_fread(void *b, long s, void *fh) { return (long)fread(b, 1, (size_t)s, (FILE *)fh); }
void plat_fclose(void *fh) { if (fh) fclose((FILE *)fh); }
