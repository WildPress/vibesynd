/* demo_render.c -- first native render milestone (port-v0.1.0 groundwork).
 *
 * Loads a real full-screen Syndicate image and its palette FROM THE USER'S OWN DATA
 * (no assets bundled) and shows it in a GPU-backed SDL2 window, proving the modern
 * render path end-to-end natively: RNC-unpack the palette, load the raw 320x200 frame,
 * convert 8-bit->RGBA through the palette, upload as a GPU texture, present.
 *
 * Usage: syn-demo [DATA_DIR]
 *   DATA_DIR defaults to the GOG install. MCONSCR.DAT (menu console screen, raw 320x200)
 *   with MSELECT.PAL (RNC-1). Press SPACE to cycle palettes, ESC/close to quit.
 */
#include "platform.h"
#include "gfx.h"
#include "rnc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_DATA \
    "C:/Program Files (x86)/GOG Galaxy/Games/Syndicate Plus/SYNDICAT/DATA"

static long load_file(const char *dir, const char *name, unsigned char *buf, long cap) {
    char path[1024];
    FILE *f;
    long n;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path); return -1; }
    n = (long)fread(buf, 1, (size_t)cap, f);
    fclose(f);
    return n;
}

/* Load a palette file: raw 768, or RNC-1 -> 768. Returns 0 on success. */
static int load_palette(const char *dir, const char *name, uint8_t out[768]) {
    unsigned char raw[8192];
    long n = load_file(dir, name, raw, sizeof raw);
    if (n < 0) return -1;
    if (n >= 4 && raw[0] == 'R' && raw[1] == 'N' && raw[2] == 'C') {
        long u = rnc_unpack(raw, n, out, 768);
        if (u != 768) { fprintf(stderr, "%s: RNC unpack -> %ld (want 768)\n", name, u); return -1; }
    } else if (n == 768) {
        memcpy(out, raw, 768);
    } else {
        fprintf(stderr, "%s: unexpected palette size %ld\n", name, n);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *dir = (argc > 1) ? argv[1] : DEFAULT_DATA;
    const char *pals[] = { "MSELECT.PAL", "HPALETTE.DAT", "HPAL01.DAT", "HPAL04.DAT" };
    const int npal = (int)(sizeof pals / sizeof pals[0]);
    int cur = 0;
    uint8_t pal[768];
    uint8_t *fb;
    unsigned char screen[SYN_W * SYN_H];

    if (load_file(dir, "MCONSCR.DAT", screen, sizeof screen) != SYN_W * SYN_H) {
        fprintf(stderr, "MCONSCR.DAT not a 320x200 raw screen\n");
        return 1;
    }
    if (load_palette(dir, pals[cur], pal) != 0) return 1;

    /* headless: `syn-demo DATA_DIR --shot out.ppm` renders one frame via the exact
     * C path (no SDL) so the conversion can be verified byte-for-byte. */
    if (argc > 3 && strcmp(argv[2], "--shot") == 0) {
        static uint32_t rgba[SYN_W * SYN_H];
        FILE *f = fopen(argv[3], "wb");
        int i;
        if (!f) { fprintf(stderr, "cannot write %s\n", argv[3]); return 1; }
        gfx_indexed_to_rgba(screen, SYN_W, SYN_H, pal, rgba);
        fprintf(f, "P6\n%d %d\n255\n", SYN_W, SYN_H);
        for (i = 0; i < SYN_W * SYN_H; i++) {
            uint32_t p = rgba[i];
            unsigned char rgb[3] = { p & 0xff, (p >> 8) & 0xff, (p >> 16) & 0xff };
            fwrite(rgb, 1, 3, f);
        }
        fclose(f);
        fprintf(stderr, "[demo] wrote %s (%dx%d) from the C render path\n", argv[3], SYN_W, SYN_H);
        return 0;
    }

    plat_video_init(SYN_W, SYN_H);
    fb = plat_framebuffer();
    memcpy(fb, screen, SYN_W * SYN_H);
    plat_set_palette(pal);

    fprintf(stderr, "[demo] rendering MCONSCR.DAT with %s. SPACE=cycle palette, ESC=quit.\n",
            pals[cur]);
    while (!plat_want_quit()) {
        int k;
        plat_present();
        while ((k = plat_poll_key()) != 0) {
            if (k == ' ') {
                cur = (cur + 1) % npal;
                if (load_palette(dir, pals[cur], pal) == 0) {
                    plat_set_palette(pal);
                    fprintf(stderr, "[demo] palette -> %s\n", pals[cur]);
                }
            }
        }
        plat_sleep_ms(16);
    }
    return 0;
}
