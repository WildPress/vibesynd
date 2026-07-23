/* net_display.c -- game-side platform backend that streams frames to a native viewer over a TCP
 * socket and reads input back. Drop-in for shm_display.c (implements platform.h + plat_present_buf
 * + syn_shm_pump_input + syn_get_mouse), used when the viewer is a separate native process (e.g. a
 * Windows .exe). The game is the server; it waits for the viewer to connect.
 */
#include "platform.h"
#include "netface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

static int g_conn = -1;
static uint8_t g_pal[768];
static uint32_t g_frame = 0;
static volatile int g_quit = 0;

/* input state, updated by the receive thread */
static volatile int g_mx, g_my, g_mb;
static int g_keys[256]; static volatile unsigned g_khead, g_ktail;

static int send_all(int fd, const void *buf, size_t n) {
    const char *p = buf; size_t off = 0;
    while (off < n) { ssize_t k = send(fd, p + off, n - off, 0); if (k <= 0) return -1; off += (size_t)k; }
    return 0;
}
static int recv_all(int fd, void *buf, size_t n) {
    char *p = buf; size_t off = 0;
    while (off < n) { ssize_t k = recv(fd, p + off, n - off, 0); if (k <= 0) return -1; off += (size_t)k; }
    return 0;
}

static void *recv_loop(void *arg) {
    syn_input_t in;
    (void)arg;
    while (!g_quit) {
        int i;
        if (recv_all(g_conn, &in, sizeof in) != 0) { g_quit = 1; break; }
        g_mx = in.mx; g_my = in.my; g_mb = in.buttons;
        for (i = 0; i < in.nkeys && i < SYN_NKEYS; i++) {
            g_keys[g_ktail % 256] = in.keys[i]; g_ktail++;
        }
    }
    return 0;
}

void plat_video_init(int w, int h) {
    int srv, one = 1;
    struct sockaddr_in a;
    pthread_t th;
    (void)w; (void)h;
    if (g_conn >= 0) return;
    srv = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_ANY); a.sin_port = htons(SYN_PORT);
    if (bind(srv, (struct sockaddr *)&a, sizeof a) != 0) { perror("bind"); return; }
    listen(srv, 1);
    fprintf(stderr, "[net] waiting for the viewer on port %d...\n", SYN_PORT);
    g_conn = accept(srv, NULL, NULL);
    close(srv);
    if (g_conn < 0) { perror("accept"); return; }
    setsockopt(g_conn, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
    fprintf(stderr, "[net] viewer connected\n");
    pthread_create(&th, 0, recv_loop, 0);
}

uint8_t *plat_framebuffer(void) { static uint8_t fb[SYN_W * SYN_H]; return fb; }
void plat_set_palette(const uint8_t *rgb768) { memcpy(g_pal, rgb768, 768); }
void plat_present(void) { plat_present_buf(plat_framebuffer()); }

void plat_present_buf(const unsigned char *buf) {
    static syn_frame_t fr;
    if (g_conn < 0 || g_quit) return;
    fr.frame = ++g_frame;
    memcpy(fr.fb, buf, SYN_W * SYN_H);
    memcpy(fr.pal, g_pal, 768);
    if (send_all(g_conn, &fr, sizeof fr) != 0) g_quit = 1;
}

/* --- input, same interface as shm_display --- */
extern unsigned char __dgroup[];
void syn_shm_pump_input(void) {
    *(short *)(__dgroup + 0x5390) = (short)g_mx;   /* g_mouse_x */
    *(short *)(__dgroup + 0x5392) = (short)g_my;   /* g_mouse_y */
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
void plat_mouse(int *x, int *y, int *b) { if (x) *x = g_mx; if (y) *y = g_my; if (b) *b = g_mb; }
void syn_get_mouse(int *x, int *y, int *b) { plat_mouse(x, y, b); }
int plat_want_quit(void) { return g_quit; }

uint32_t plat_ticks_ms(void) { return 0; }
void plat_sleep_ms(uint32_t ms) { (void)ms; }
void plat_audio_init(void) {}
void plat_play_sound(int id) { (void)id; }
void plat_play_music(int t) { (void)t; }
void *plat_fopen(const char *p, const char *m) { return fopen(p, m); }
long plat_fread(void *b, long s, void *fh) { return (long)fread(b, 1, (size_t)s, (FILE *)fh); }
void plat_fclose(void *fh) { if (fh) fclose((FILE *)fh); }
