/* platform_sdl.c -- SDL2 + GPU backend for the native port (implements platform.h).
 *
 * Video: the game's 8-bit 320x200 framebuffer is converted through the palette to RGBA
 * each present and uploaded to a GPU streaming texture, drawn scaled into a resizable
 * window (hardware-accelerated, so scaling/vsync are free). This is the modern stand-in
 * for the mode-X vga_planar_present.
 *
 * Only video + input + timing are implemented here for the first-render milestone; audio
 * and file I/O keep the simple stdio/no-op behaviour.
 */
#include "platform.h"
#include "gfx.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SDL_Window   *g_win;
static SDL_Renderer *g_ren;
static SDL_Texture  *g_tex;
static uint8_t       g_fb[SYN_W * SYN_H];      /* the 8-bit game framebuffer */
static uint8_t       g_pal[768];               /* 6-bit VGA palette (0..63)  */
static uint32_t      g_rgba[SYN_W * SYN_H];    /* conversion scratch         */
static int           g_quit;

/* small key queue fed by plat_present's event pump */
static int g_keys[64], g_khead, g_ktail;
static int g_mx, g_my, g_mbtn;

void plat_video_init(int width, int height) {
    int scale = 3;
    (void)width; (void)height;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }
    g_win = SDL_CreateWindow("Syndicate (native port)",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             SYN_W * scale, SYN_H * scale, SDL_WINDOW_RESIZABLE);
    g_ren = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    /* keep the 320x200 aspect when the window is resized; letterbox the rest */
    SDL_RenderSetLogicalSize(g_ren, SYN_W, SYN_H);
    g_tex = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_ABGR8888,
                              SDL_TEXTUREACCESS_STREAMING, SYN_W, SYN_H);
}

uint8_t *plat_framebuffer(void) { return g_fb; }

void plat_set_palette(const uint8_t *rgb768) { memcpy(g_pal, rgb768, 768); }

static void pump_events(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            g_quit = 1; break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE) g_quit = 1;
            g_keys[g_ktail] = e.key.keysym.sym;
            g_ktail = (g_ktail + 1) & 63;
            break;
        case SDL_MOUSEMOTION:
            g_mx = e.motion.x; g_my = e.motion.y; break;
        case SDL_MOUSEBUTTONDOWN: g_mbtn |= SDL_BUTTON(e.button.button); break;
        case SDL_MOUSEBUTTONUP:   g_mbtn &= ~SDL_BUTTON(e.button.button); break;
        }
    }
}

void plat_present(void) {
    gfx_indexed_to_rgba(g_fb, SYN_W, SYN_H, g_pal, g_rgba);
    SDL_UpdateTexture(g_tex, NULL, g_rgba, SYN_W * (int)sizeof(uint32_t));
    SDL_RenderClear(g_ren);
    SDL_RenderCopy(g_ren, g_tex, NULL, NULL);
    SDL_RenderPresent(g_ren);
    pump_events();
}

int plat_poll_key(void) {
    int k;
    if (g_khead == g_ktail) return 0;
    k = g_keys[g_khead];
    g_khead = (g_khead + 1) & 63;
    return k;
}

void plat_mouse(int *x, int *y, int *buttons) {
    if (x) *x = g_mx;
    if (y) *y = g_my;
    if (buttons) *buttons = g_mbtn;
}

int plat_want_quit(void) { return g_quit; }

uint32_t plat_ticks_ms(void) { return SDL_GetTicks(); }
void     plat_sleep_ms(uint32_t ms) { SDL_Delay(ms); }

void plat_audio_init(void) {}
void plat_play_sound(int id) { (void)id; }
void plat_play_music(int track) { (void)track; }

void *plat_fopen(const char *path, const char *mode) { return fopen(path, mode); }
long  plat_fread(void *buf, long size, void *fh) { return (long)fread(buf, 1, (size_t)size, (FILE *)fh); }
void  plat_fclose(void *fh) { if (fh) fclose((FILE *)fh); }
