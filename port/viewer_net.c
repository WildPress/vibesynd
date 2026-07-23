/* viewer_net.c -- SDL viewer that connects to the game over TCP and presents its frames in a
 * window, sending input back. Cross-platform: compiles natively for Windows (MinGW + winsock),
 * giving a real Win32 window, and for Linux. Talks to net_display.c (the game is the server).
 */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "netface.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET sock_t;
  #define CLOSESOCK closesocket
  #define BADSOCK INVALID_SOCKET
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef int sock_t;
  #define CLOSESOCK close
  #define BADSOCK (-1)
#endif

static int recv_all(sock_t fd, void *buf, int n) {
    char *p = (char *)buf; int off = 0;
    while (off < n) { int k = (int)recv(fd, p + off, n - off, 0); if (k <= 0) return -1; off += k; }
    return 0;
}
static int send_all(sock_t fd, const void *buf, int n) {
    const char *p = (const char *)buf; int off = 0;
    while (off < n) { int k = (int)send(fd, p + off, n - off, 0); if (k <= 0) return -1; off += k; }
    return 0;
}

static int map_key(SDL_Keysym k) {
    int sym = k.sym;
    switch (sym) {
    case SDLK_ESCAPE: return 0x011b; case SDLK_RETURN: return 0x1c0d;
    case SDLK_SPACE: return 0x3920;  case SDLK_BACKSPACE: return 0x0e08;
    case SDLK_TAB: return 0x0f09;
    case SDLK_UP: return 0x4800; case SDLK_DOWN: return 0x5000;
    case SDLK_LEFT: return 0x4b00; case SDLK_RIGHT: return 0x4d00;
    default: break;
    }
    if (sym >= SDLK_F1 && sym <= SDLK_F10) return (0x3b + (sym - SDLK_F1)) << 8;
    if (sym >= 0x20 && sym < 0x7f) { int a = sym; if ((k.mod & KMOD_SHIFT) && a >= 'a' && a <= 'z') a -= 32; return a & 0xff; }
    return 0;
}

/* recv thread: keep the latest frame */
static syn_frame_t g_latest;
static SDL_mutex *g_lock;
static volatile int g_have = 0, g_stop = 0;
static sock_t g_sock;

static int recv_thread(void *arg) {
    static syn_frame_t tmp;
    (void)arg;
    while (!g_stop) {
        if (recv_all(g_sock, &tmp, sizeof tmp) != 0) { g_stop = 1; break; }
        SDL_LockMutex(g_lock); memcpy(&g_latest, &tmp, sizeof tmp); g_have = 1; SDL_UnlockMutex(g_lock);
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    struct sockaddr_in a;
    SDL_Window *win; SDL_Renderer *ren; SDL_Texture *tex; SDL_Thread *rt;
    static uint32_t rgba[SYN_W * SYN_H];
    uint32_t last = 0xffffffff;
    int scale = 3, tries;
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }

    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET; a.sin_port = htons(SYN_PORT);
    a.sin_addr.s_addr = inet_addr(host);
    g_sock = socket(AF_INET, SOCK_STREAM, 0);
    for (tries = 0; tries < 100; tries++) {          /* wait for the game to start listening */
        if (connect(g_sock, (struct sockaddr *)&a, sizeof a) == 0) break;
        SDL_Delay(100);
    }
    if (tries >= 100) { fprintf(stderr, "viewer: could not connect to the game on %s:%d\n", host, SYN_PORT); return 1; }
    { int one = 1; setsockopt(g_sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof one); }

    win = SDL_CreateWindow("Syndicate (native port)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           SYN_W * scale, SYN_H * scale, SDL_WINDOW_RESIZABLE);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren, SYN_W, SYN_H);
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SYN_W, SYN_H);
    g_lock = SDL_CreateMutex();
    rt = SDL_CreateThread(recv_thread, "recv", 0);

    while (!g_stop) {
        syn_input_t in; SDL_Event e;
        int wx, wy; float lx, ly; uint32_t bs;
        memset(&in, 0, sizeof in);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) g_stop = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = map_key(e.key.keysym);
                if (e.key.keysym.sym == SDLK_ESCAPE && (e.key.keysym.mod & KMOD_SHIFT)) g_stop = 1;
                if (code && in.nkeys < SYN_NKEYS) in.keys[in.nkeys++] = code;
            }
        }
        bs = SDL_GetMouseState(&wx, &wy);
        SDL_RenderWindowToLogical(ren, wx, wy, &lx, &ly);
        in.mx = (int)lx; in.my = (int)ly; in.buttons = (int)bs;
        if (send_all(g_sock, &in, sizeof in) != 0) g_stop = 1;

        SDL_LockMutex(g_lock);
        if (g_have && g_latest.frame != last) {
            last = g_latest.frame;
            gfx_indexed_to_rgba(g_latest.fb, SYN_W, SYN_H, g_latest.pal, rgba);
            SDL_UpdateTexture(tex, NULL, rgba, SYN_W * (int)sizeof(uint32_t));
        }
        SDL_UnlockMutex(g_lock);
        SDL_RenderClear(ren); SDL_RenderCopy(ren, tex, NULL, NULL); SDL_RenderPresent(ren);
        SDL_Delay(10);
    }
    g_stop = 1; CLOSESOCK(g_sock);
    SDL_WaitThread(rt, NULL);
    SDL_DestroyWindow(win); SDL_Quit();
    return 0;
}
