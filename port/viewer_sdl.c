/* viewer_sdl.c -- 64-bit SDL window that presents the game's shared-memory frame and feeds input
 * back. Runs as a separate process from the 32-bit game, so it can link the SDL already installed.
 *
 * It waits for the game to publish the shm, then each ~16ms converts the 8-bit frame through the
 * palette to a GPU texture and presents it, and pumps keyboard/mouse into the shm (BIOS-style
 * scancode<<8|ascii for keys, logical 320x200 coords for the mouse).
 */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "shmface.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

static syn_shm_t *open_shm(void) {
    int tries;
    for (tries = 0; tries < 200; tries++) {          /* wait up to ~10s for the game */
        int fd = shm_open(SYN_SHM_NAME, O_RDWR, 0666);
        if (fd >= 0) {
            syn_shm_t *s = mmap(NULL, sizeof(syn_shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            if (s != MAP_FAILED) {
                while (s->magic != SYN_SHM_MAGIC && tries++ < 200) SDL_Delay(50);
                return s;
            }
        }
        SDL_Delay(50);
    }
    return NULL;
}

/* SDL key -> BIOS-style (scancode<<8)|ascii, enough for menus (Esc/Enter/F-keys/arrows/typing) */
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
    if (sym >= 0x20 && sym < 0x7f) {          /* printable ascii */
        int a = sym;
        if ((k.mod & KMOD_SHIFT) && a >= 'a' && a <= 'z') a -= 32;
        return a & 0xff;
    }
    return 0;
}

int main(int argc, char **argv) {
    syn_shm_t *shm;
    SDL_Window *win; SDL_Renderer *ren; SDL_Texture *tex;
    static uint32_t rgba[SYN_W * SYN_H];
    uint32_t last = 0xffffffff;
    int scale = 3;
    (void)argc; (void)argv;

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    shm = open_shm();
    if (!shm) { fprintf(stderr, "viewer: no game shm (start the game first)\n"); return 1; }

    win = SDL_CreateWindow("Syndicate (native port)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           SYN_W * scale, SYN_H * scale, SDL_WINDOW_RESIZABLE);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren, SYN_W, SYN_H);
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SYN_W, SYN_H);

    while (!shm->quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) shm->quit = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = map_key(e.key.keysym);
                if (e.key.keysym.sym == SDLK_ESCAPE && (e.key.keysym.mod & KMOD_SHIFT)) shm->quit = 1;
                if (code) {
                    uint32_t t = shm->key_tail;
                    shm->keys[t % SYN_KEYS] = code;
                    shm->key_tail = t + 1;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                float lx, ly; SDL_RenderWindowToLogical(ren, e.motion.x, e.motion.y, &lx, &ly);
                shm->mouse_x = (int)lx; shm->mouse_y = (int)ly;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                shm->mouse_buttons |= SDL_BUTTON(e.button.button);
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                shm->mouse_buttons &= ~SDL_BUTTON(e.button.button);
            }
        }
        if (shm->frame != last) {
            last = shm->frame;
            gfx_indexed_to_rgba((const uint8_t *)shm->fb, SYN_W, SYN_H, (const uint8_t *)shm->pal, rgba);
            SDL_UpdateTexture(tex, NULL, rgba, SYN_W * (int)sizeof(uint32_t));
        }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
