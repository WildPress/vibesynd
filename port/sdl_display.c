/* sdl_display.c -- in-process SDL2 display backend (implements platform.h + plat_present_buf +
 * the input interface). Unlike shm_display.c + viewer_sdl.c (a 32-bit game feeding a separate
 * 64-bit SDL viewer over shared memory), this links SDL straight into the game process, which a
 * native build can do: the game is 32-bit and so is the MinGW SDL2 it links. One window, one exe.
 *
 * Threading: the boot harness runs the game on one thread and calls plat_present_buf from the
 * display thread. SDL wants all its video calls -- init, window, event pump, render -- on a single
 * thread, so this lazily initialises SDL on the FIRST plat_present_buf and does everything (event
 * pump + present) there. The game thread only ever touches the input/palette globals below.
 */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "platform.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

void plat_present_buf(const unsigned char *buf);   /* defined below */

static SDL_Window   *g_win;
static SDL_Renderer *g_ren;
static SDL_Texture  *g_tex;
static int           g_ready = 0;

static uint8_t  g_pal[768];
static uint32_t g_rgba[SYN_W * SYN_H];

/* input state, written by the SDL event pump (display thread), read by the game via the pump
 * functions below. Plain ints -- the same benign cross-thread sharing the shm backend used. */
static volatile int g_mouse_x, g_mouse_y, g_mouse_b;
static int          g_keys[256];
static volatile unsigned g_khead, g_ktail;
static volatile int g_quit;

/* SDL key -> BIOS-style (scancode<<8)|ascii, enough for menus + typing (from viewer_sdl.c) */
static int map_key(SDL_Keysym k) {
    int sym = k.sym;
    switch (sym) {
    case SDLK_ESCAPE:    return 0x011b;
    case SDLK_RETURN:    return 0x1c0d;
    case SDLK_SPACE:     return 0x3920;
    case SDLK_BACKSPACE: return 0x0e08;
    case SDLK_TAB:       return 0x0f09;
    case SDLK_UP:        return 0x4800;
    case SDLK_DOWN:      return 0x5000;
    case SDLK_LEFT:      return 0x4b00;
    case SDLK_RIGHT:     return 0x4d00;
    default: break;
    }
    if (sym >= SDLK_F1 && sym <= SDLK_F10) return (0x3b + (sym - SDLK_F1)) << 8;
    if (sym >= 0x20 && sym < 0x7f) {
        int a = sym;
        if ((k.mod & KMOD_SHIFT) && a >= 'a' && a <= 'z') a -= 32;
        return a & 0xff;
    }
    return 0;
}

void plat_video_init(int w, int h) { (void)w; (void)h; }   /* real init is lazy, on the display thread */

static void sdl_lazy_init(void) {
    int scale = 3;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return; }
    g_win = SDL_CreateWindow("Syndicate (native port)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             SYN_W * scale, SYN_H * scale, SDL_WINDOW_RESIZABLE);
    g_ren = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(g_ren, SYN_W, SYN_H);
    g_tex = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SYN_W, SYN_H);
    g_ready = (g_win && g_ren && g_tex);
    if (!g_ready) fprintf(stderr, "SDL window/renderer/texture failed: %s\n", SDL_GetError());
}

static void sdl_pump_events(void) {
    SDL_Event e;
    int wx, wy; float lx, ly; uint32_t bs;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) g_quit = 1;
        else if (e.type == SDL_KEYDOWN) {
            int code = map_key(e.key.keysym);
            if (e.key.keysym.sym == SDLK_ESCAPE && (e.key.keysym.mod & KMOD_SHIFT)) g_quit = 1;
            if (code) { g_keys[g_ktail % 256] = code; g_ktail++; }
        }
    }
    bs = SDL_GetMouseState(&wx, &wy);
    SDL_RenderWindowToLogical(g_ren, wx, wy, &lx, &ly);
    g_mouse_x = (int)lx; g_mouse_y = (int)ly; g_mouse_b = (int)bs;
}

uint8_t *plat_framebuffer(void) { static uint8_t fb[SYN_W * SYN_H]; return fb; }
void plat_set_palette(const uint8_t *rgb768) { memcpy(g_pal, rgb768, 768); }
void plat_present(void) { plat_present_buf(plat_framebuffer()); }

void plat_present_buf(const unsigned char *buf) {
    if (!g_ready) { sdl_lazy_init(); if (!g_ready) return; }
    sdl_pump_events();
    gfx_indexed_to_rgba(buf, SYN_W, SYN_H, g_pal, g_rgba);
    SDL_UpdateTexture(g_tex, NULL, g_rgba, SYN_W * (int)sizeof(uint32_t));
    SDL_RenderClear(g_ren);
    SDL_RenderCopy(g_ren, g_tex, NULL, NULL);
    SDL_RenderPresent(g_ren);
}

/* --- input: same interface + DGROUP offsets as shm_display.c --- */
extern unsigned char __dgroup[];
void syn_shm_pump_input(void) {
    *(short *)(__dgroup + 0x5390) = (short)g_mouse_x;   /* g_mouse_x */
    *(short *)(__dgroup + 0x5392) = (short)g_mouse_y;   /* g_mouse_y */
    while (g_khead != g_ktail) {
        int code = g_keys[g_khead % 256]; unsigned char sc = (code >> 8) & 0xff;
        g_khead++;
        if (!sc) sc = code & 0xff;
        if (sc) { __dgroup[0x537e] = sc; __dgroup[0xe284 + sc] = 1; }
    }
}
int plat_poll_key(void) {
    if (g_khead == g_ktail) return 0;
    return g_keys[(g_khead++) % 256];
}
void plat_mouse(int *x, int *y, int *b) { if (x) *x = g_mouse_x; if (y) *y = g_mouse_y; if (b) *b = g_mouse_b; }
void syn_get_mouse(int *x, int *y, int *b) { plat_mouse(x, y, b); }
int plat_want_quit(void) { return g_quit; }

uint32_t plat_ticks_ms(void) { return SDL_GetTicks(); }
void plat_sleep_ms(uint32_t ms) { SDL_Delay(ms); }
void plat_audio_init(void) {}
void plat_play_sound(int id) { (void)id; }
void plat_play_music(int t) { (void)t; }
void *plat_fopen(const char *p, const char *m) { return fopen(p, m); }
long plat_fread(void *b, long s, void *fh) { return (long)fread(b, 1, (size_t)s, (FILE *)fh); }
void plat_fclose(void *fh) { if (fh) fclose((FILE *)fh); }
